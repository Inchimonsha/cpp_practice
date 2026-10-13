// m11_networking_io/ex01_sockets — TCP-kliyent/server cherez RAII (Winsock/POSIX).
// Standart: C++23. Zapusk: ./run.ps1 m11_networking_io/ex01_sockets -Extra '-lws2_32'
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

#include <cstdint>
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
    if (getsockname(server.get(), reinterpret_cast<sockaddr*>(&address), &length) != 0) {
        throw std::runtime_error("getsockname() failed");
    }
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
    std::println("server listening on 127.0.0.1 (ephemeral port)");

    std::string server_received;
    std::jthread server([&listener, &server_received] {
        Socket connection(accept(listener.get(), nullptr, nullptr));
        char buffer[128] = {};
        const int received = recv(connection.get(), buffer, sizeof(buffer), 0);
        server_received.assign(buffer, received > 0 ? static_cast<std::size_t>(received) : 0);
        const int sent = send(connection.get(), server_received.data(),
                              static_cast<int>(server_received.size()), 0);
        (void)sent;
    });

    Socket client = connect_loopback(port);
    const std::string request = "ping";
    const int sent = send(client.get(), request.data(), static_cast<int>(request.size()), 0);
    (void)sent;

    char reply[128] = {};
    const int received = recv(client.get(), reply, sizeof(reply), 0);
    const std::string response(reply, received > 0 ? static_cast<std::size_t>(received) : 0);

    server.join();

    std::println("client -> server: \"{}\"", request);
    std::println("server received: \"{}\"", server_received);
    std::println("client <- server: \"{}\"", response);

    const bool ok = server_received == "ping" && response == "ping";
    std::cout << "sockets done\n";
    return ok ? 0 : 1;
}
