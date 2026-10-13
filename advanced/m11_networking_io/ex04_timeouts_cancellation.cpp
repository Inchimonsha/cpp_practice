// m11_networking_io/ex04_timeouts_cancellation — taimauty i otmena (stop_token).
// Standart: C++23. Zapusk: ./run.ps1 m11_networking_io/ex04_timeouts_cancellation -Extra '-lws2_32'
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

bool poll_readable(socket_t descriptor, int timeout_ms) {
    fd_set read_fds;
    FD_ZERO(&read_fds);
    FD_SET(descriptor, &read_fds);
    timeval timeout{timeout_ms / 1000, (timeout_ms % 1000) * 1000};
    const int ready = select(static_cast<int>(descriptor) + 1, &read_fds, nullptr, nullptr, &timeout);
    return ready > 0 && FD_ISSET(descriptor, &read_fds);
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
    if (listen(server.get(), 4) != 0) {
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
    Socket client = connect_loopback(port);
    Socket connection(accept(listener.get(), nullptr, nullptr));

    const bool ready_before = poll_readable(connection.get(), 150);
    std::println("timeout (150 ms): readable = {}", ready_before);

    const std::string message = "data";
    send(client.get(), message.data(), static_cast<int>(message.size()), 0);

    const bool ready_after = poll_readable(connection.get(), 2000);
    std::println("after send: readable = {}", ready_after);

    std::string received;
    if (ready_after) {
        char buffer[64] = {};
        const int count = recv(connection.get(), buffer, sizeof(buffer), 0);
        received.assign(buffer, count > 0 ? static_cast<std::size_t>(count) : 0);
    }
    std::println("received: \"{}\"", received);

    std::jthread worker([&connection](std::stop_token token) {
        while (!token.stop_requested()) {
            (void)poll_readable(connection.get(), 50);
        }
        std::println("worker: stop_token observed, loop exited");
    });
    std::this_thread::sleep_for(std::chrono::milliseconds(30));
    worker.request_stop();
    worker.join();

    const bool ok = !ready_before && ready_after && received == "data";
    std::cout << "timeouts/cancellation done\n";
    return ok ? 0 : 1;
}
