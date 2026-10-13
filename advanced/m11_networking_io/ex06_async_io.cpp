// m11_networking_io/ex06_async_io — kompoziciya asinhronnyh operaciy nad soketami.
// Standart: C++23. Zapusk: ./run.ps1 m11_networking_io/ex06_async_io -Extra '-lws2_32'
#ifdef _WIN32
#  define WIN32_LEAN_AND_MEAN
#  include <winsock2.h>
#  include <ws2tcpip.h>
#else
#  include <arpa/inet.h>
#  include <netinet/in.h>
#  include <sys/socket.h>
#  include <unistd.h>
#endif

#include <cctype>
#include <cstdint>
#include <cstring>
#include <expected>
#include <iostream>
#include <optional>
#include <print>
#include <stdexcept>
#include <string>
#include <thread>

#ifdef _WIN32
using socket_t = SOCKET;
using socklen_type = int;
inline constexpr socket_t k_invalid = INVALID_SOCKET;
inline int close_socket(socket_t s) { return closesocket(s); }
struct WsaGuard {
    WsaGuard() {
        WSADATA data;
        if (WSAStartup(MAKEWORD(2, 2), &data) != 0) {
            throw std::runtime_error("WSAStartup failed");
        }
    }
    ~WsaGuard() { WSACleanup(); }
    WsaGuard(const WsaGuard&) = delete;
    WsaGuard& operator=(const WsaGuard&) = delete;
};
#else
using socket_t = int;
using socklen_type = socklen_t;
inline constexpr socket_t k_invalid = -1;
inline int close_socket(socket_t s) { return ::close(s); }
struct WsaGuard {
    WsaGuard() = default;
};
#endif

class Socket {
public:
    Socket() = default;
    explicit Socket(socket_t descriptor) : fd_(descriptor) {}
    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;
    Socket(Socket&& other) noexcept : fd_(other.fd_) { other.fd_ = k_invalid; }
    Socket& operator=(Socket&& other) noexcept {
        if (this != &other) {
            if (fd_ != k_invalid) {
                close_socket(fd_);
            }
            fd_ = other.fd_;
            other.fd_ = k_invalid;
        }
        return *this;
    }
    ~Socket() {
        if (fd_ != k_invalid) {
            close_socket(fd_);
        }
    }
    socket_t get() const { return fd_; }

private:
    socket_t fd_ = k_invalid;
};

bool poll_readable(socket_t descriptor, int timeout_ms) {
    fd_set read_fds;
    FD_ZERO(&read_fds);
    FD_SET(descriptor, &read_fds);
    timeval timeout{timeout_ms / 1000, (timeout_ms % 1000) * 1000};
    const int ready = select(static_cast<int>(descriptor) + 1, &read_fds, nullptr, nullptr, &timeout);
    return ready > 0 && FD_ISSET(descriptor, &read_fds);
}

namespace async {

struct read_sender {
    socket_t fd;
    int timeout_ms;

    template <class Receiver>
    struct operation {
        socket_t fd;
        int timeout_ms;
        Receiver receiver;

        void start() {
            if (!poll_readable(fd, timeout_ms)) {
                receiver.set_error("timeout");
                return;
            }
            char buffer[128] = {};
            const int count = recv(fd, buffer, sizeof(buffer), 0);
            if (count <= 0) {
                receiver.set_error("closed");
                return;
            }
            receiver.set_value(std::string(buffer, static_cast<std::size_t>(count)));
        }
    };

    template <class Receiver>
    operation<Receiver> connect(Receiver receiver) const {
        return operation<Receiver>{fd, timeout_ms, std::move(receiver)};
    }
};

template <class Sender, class F>
struct then_sender {
    Sender sender;
    F f;

    template <class Receiver>
    struct operation {
        Sender sender;
        F f;
        Receiver receiver;

        struct inner {
            F f;
            Receiver* out;
            void set_value(std::string value) { out->set_value(f(std::move(value))); }
            void set_error(std::string error) { out->set_error(std::move(error)); }
        };

        void start() {
            inner sink{f, &receiver};
            auto op = sender.connect(std::move(sink));
            op.start();
        }
    };

    template <class Receiver>
    operation<Receiver> connect(Receiver receiver) const {
        return operation<Receiver>{sender, f, std::move(receiver)};
    }
};

template <class Sender, class F>
then_sender<Sender, F> then(Sender sender, F f) {
    return then_sender<Sender, F>{std::move(sender), std::move(f)};
}

template <class S1, class S2, class F>
struct when_all_sender {
    S1 first;
    S2 second;
    F combine;

    template <class Receiver>
    struct operation {
        S1 first;
        S2 second;
        F combine;
        Receiver receiver;
        std::optional<std::string> left;
        std::optional<std::string> right;

        void deliver() {
            if (left && right) {
                receiver.set_value(combine(*left, *right));
            }
        }

        struct left_handler {
            operation* self;
            void set_value(std::string value) {
                self->left = std::move(value);
                self->deliver();
            }
            void set_error(std::string error) { self->receiver.set_error(std::move(error)); }
        };
        struct right_handler {
            operation* self;
            void set_value(std::string value) {
                self->right = std::move(value);
                self->deliver();
            }
            void set_error(std::string error) { self->receiver.set_error(std::move(error)); }
        };

        void start() {
            auto first_op = first.connect(left_handler{this});
            auto second_op = second.connect(right_handler{this});
            first_op.start();
            second_op.start();
        }
    };

    template <class Receiver>
    operation<Receiver> connect(Receiver receiver) const {
        return operation<Receiver>{first, second, combine, std::move(receiver),
                                   std::nullopt, std::nullopt};
    }
};

template <class S1, class S2, class F>
when_all_sender<S1, S2, F> when_all(S1 first, S2 second, F combine) {
    return when_all_sender<S1, S2, F>{std::move(first), std::move(second), std::move(combine)};
}

template <class Sender>
std::expected<std::string, std::string> sync_wait(Sender sender) {
    std::optional<std::string> value;
    std::optional<std::string> error;
    struct receiver {
        std::optional<std::string>* value;
        std::optional<std::string>* error;
        void set_value(std::string v) { *value = std::move(v); }
        void set_error(std::string e) { *error = std::move(e); }
    };
    auto op = sender.connect(receiver{&value, &error});
    op.start();
    if (error) {
        return std::unexpected(*error);
    }
    return *value;
}

}  // namespace async

std::string to_upper(std::string text) {
    for (char& character : text) {
        character = static_cast<char>(std::toupper(static_cast<unsigned char>(character)));
    }
    return text;
}

Socket listen_loopback(std::uint16_t& port) {
    Socket server(socket(AF_INET, SOCK_STREAM, IPPROTO_TCP));
    if (server.get() == k_invalid) {
        throw std::runtime_error("socket() failed");
    }
    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    address.sin_port = 0;
    if (bind(server.get(), reinterpret_cast<sockaddr*>(&address), sizeof(address)) != 0) {
        throw std::runtime_error("bind() failed");
    }
    socklen_type length = sizeof(address);
    getsockname(server.get(), reinterpret_cast<sockaddr*>(&address), &length);
    port = ntohs(address.sin_port);
    if (listen(server.get(), 8) != 0) {
        throw std::runtime_error("listen() failed");
    }
    return server;
}

Socket connect_loopback(std::uint16_t port) {
    Socket client(socket(AF_INET, SOCK_STREAM, IPPROTO_TCP));
    if (client.get() == k_invalid) {
        throw std::runtime_error("socket() failed");
    }
    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    address.sin_port = htons(port);
    if (connect(client.get(), reinterpret_cast<sockaddr*>(&address), sizeof(address)) != 0) {
        throw std::runtime_error("connect() failed");
    }
    return client;
}

int main() {
    WsaGuard wsa;

    std::uint16_t port = 0;
    Socket listener = listen_loopback(port);

    std::jthread server([&listener] {
        for (int index = 0; index < 4; ++index) {
            Socket connection(accept(listener.get(), nullptr, nullptr));
            if (index == 3) {
                continue;  // zamknut' bez dannyh -> kliyent poluchit "closed"
            }
            const char* text = (index == 0) ? "hello" : (index == 1) ? "world" : "boom";
            send(connection.get(), text, static_cast<int>(std::strlen(text)), 0);
        }
    });

    Socket first = connect_loopback(port);
    Socket second = connect_loopback(port);
    Socket third = connect_loopback(port);
    Socket closed = connect_loopback(port);

    const auto upper_result =
        async::sync_wait(async::then(async::read_sender{first.get(), 2000}, to_upper));
    std::println("then(read_sender) = {}",
                 upper_result ? *upper_result : upper_result.error());

    const auto combined_result = async::sync_wait(
        async::when_all(async::read_sender{second.get(), 2000},
                        async::read_sender{third.get(), 2000},
                        [](std::string a, std::string b) { return a + "+" + b; }));
    std::println("when_all = {}",
                 combined_result ? *combined_result : combined_result.error());

    const auto error_result = async::sync_wait(async::read_sender{closed.get(), 500});
    std::println("error = {}", error_result ? *error_result : error_result.error());

    server.join();

    const bool ok = upper_result.has_value() && *upper_result == "HELLO" &&
                    combined_result.has_value() && *combined_result == "world+boom" &&
                    !error_result.has_value() && error_result.error() == "closed";
    std::cout << "async io done\n";
    return ok ? 0 : 1;
}
