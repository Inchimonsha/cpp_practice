// m11_networking_io/ex02_event_loop — reaktor na select (multiplexirovanie).
// Standart: C++23. Zapusk: ./run.ps1 m11_networking_io/ex02_event_loop -Extra '-lws2_32'
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

#include <chrono>
#include <cstdint>
#include <cstring>
#include <iostream>
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

Socket make_tcp() {
    Socket s(socket(AF_INET, SOCK_STREAM, IPPROTO_TCP));
    if (s.get() == k_invalid) {
        throw std::runtime_error("socket() failed");
    }
    return s;
}

Socket listen_loopback(std::uint16_t& port) {
    Socket server = make_tcp();
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
    if (listen(server.get(), 4) != 0) {
        throw std::runtime_error("listen() failed");
    }
    return server;
}

Socket connect_loopback(std::uint16_t port) {
    Socket client = make_tcp();
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
    Socket connection;

    std::jthread client([port] {
        Socket socket_to_server = connect_loopback(port);
        for (const char* message : {"msg1\n", "msg2\n", "msg3\n"}) {
            send(socket_to_server.get(), message, static_cast<int>(std::strlen(message)), 0);
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        }
    });

    std::string received;
    bool accepted = false;
    bool done = false;
    int idle_ticks = 0;

    while (!done && idle_ticks < 100) {
        fd_set read_fds;
        FD_ZERO(&read_fds);
        FD_SET(listener.get(), &read_fds);
        socket_t max_fd = listener.get();
        if (connection.get() != k_invalid) {
            FD_SET(connection.get(), &read_fds);
            if (connection.get() > max_fd) {
                max_fd = connection.get();
            }
        }

        timeval timeout{1, 0};
        const int ready = select(static_cast<int>(max_fd) + 1, &read_fds, nullptr, nullptr, &timeout);
        if (ready <= 0) {
            ++idle_ticks;
            continue;
        }

        if (FD_ISSET(listener.get(), &read_fds)) {
            Socket peer(accept(listener.get(), nullptr, nullptr));
            if (peer.get() != k_invalid) {
                connection = std::move(peer);
                accepted = true;
                std::println("reactor: accepted connection");
            }
        }

        if (connection.get() != k_invalid && FD_ISSET(connection.get(), &read_fds)) {
            char buffer[128] = {};
            const int count = recv(connection.get(), buffer, sizeof(buffer), 0);
            if (count <= 0) {
                done = true;
            } else {
                received.append(buffer, static_cast<std::size_t>(count));
            }
        }
    }

    client.join();

    std::size_t messages = 0;
    std::size_t start = 0;
    std::println("reactor: received messages:");
    while (true) {
        const std::size_t newline = received.find('\n', start);
        if (newline == std::string::npos) {
            break;
        }
        std::println("  {}", received.substr(start, newline - start));
        ++messages;
        start = newline + 1;
    }

    const bool ok = accepted && messages == 3;
    std::cout << "event loop done\n";
    return ok ? 0 : 1;
}
