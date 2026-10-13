// m11_networking_io/ex05_http_server — minimal'nyy HTTP/1.1 server.
// Standart: C++23. Zapusk: ./run.ps1 m11_networking_io/ex05_http_server -Extra '-lws2_32'
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
#include <format>
#include <iostream>
#include <map>
#include <optional>
#include <print>
#include <sstream>
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

struct HttpRequest {
    std::string method;
    std::string path;
    std::string version;
    std::map<std::string, std::string> headers;
};

std::string trim(const std::string& text) {
    const std::size_t first = text.find_first_not_of(" \t");
    if (first == std::string::npos) {
        return "";
    }
    const std::size_t last = text.find_last_not_of(" \t");
    return text.substr(first, last - first + 1);
}

std::optional<HttpRequest> parse_request(const std::string& raw) {
    const std::size_t line_end = raw.find("\r\n");
    if (line_end == std::string::npos) {
        return std::nullopt;
    }
    HttpRequest request;
    std::istringstream stream(raw.substr(0, line_end));
    if (!(stream >> request.method >> request.path >> request.version)) {
        return std::nullopt;
    }
    std::size_t position = line_end + 2;
    while (position < raw.size()) {
        const std::size_t end = raw.find("\r\n", position);
        if (end == std::string::npos || end == position) {
            break;
        }
        const std::string header = raw.substr(position, end - position);
        const std::size_t colon = header.find(':');
        if (colon != std::string::npos) {
            request.headers[trim(header.substr(0, colon))] = trim(header.substr(colon + 1));
        }
        position = end + 2;
    }
    return request;
}

std::string build_response(int status, const std::string& reason,
                           const std::string& content_type, const std::string& body) {
    std::string response = std::format("HTTP/1.1 {} {}\r\n", status, reason);
    response += std::format("Content-Type: {}\r\n", content_type);
    response += std::format("Content-Length: {}\r\n", body.size());
    response += "Connection: close\r\n\r\n";
    response += body;
    return response;
}

std::string route(const HttpRequest& request) {
    if (request.method == "GET" && request.path == "/") {
        return build_response(200, "OK", "text/plain", "root");
    }
    if (request.method == "GET" && request.path == "/hello") {
        return build_response(200, "OK", "text/plain", "Hello, HTTP!");
    }
    return build_response(404, "Not Found", "text/plain", "not found");
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

std::string http_get(std::uint16_t port, const std::string& path) {
    Socket client(socket(AF_INET, SOCK_STREAM, IPPROTO_TCP));
    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    address.sin_port = htons(port);
    if (connect(client.get(), reinterpret_cast<sockaddr*>(&address), sizeof(address)) != 0) {
        throw std::runtime_error("connect() failed");
    }
    const std::string request =
        std::format("GET {} HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n", path);
    send(client.get(), request.data(), static_cast<int>(request.size()), 0);

    std::string response;
    char buffer[256] = {};
    int count = 0;
    while ((count = recv(client.get(), buffer, sizeof(buffer), 0)) > 0) {
        response.append(buffer, static_cast<std::size_t>(count));
    }
    return response;
}

int status_code(const std::string& response) {
    const std::size_t space = response.find(' ');
    if (space == std::string::npos) {
        return 0;
    }
    return std::stoi(response.substr(space + 1, 3));
}

std::string body_of(const std::string& response) {
    const std::size_t separator = response.find("\r\n\r\n");
    if (separator == std::string::npos) {
        return "";
    }
    return response.substr(separator + 4);
}

int main() {
    WsaGuard wsa;

    std::uint16_t port = 0;
    Socket listener = listen_loopback(port);

    std::jthread server([&listener] {
        for (int i = 0; i < 2; ++i) {
            Socket connection(accept(listener.get(), nullptr, nullptr));
            std::string raw;
            char buffer[256] = {};
            while (raw.find("\r\n\r\n") == std::string::npos) {
                const int count = recv(connection.get(), buffer, sizeof(buffer), 0);
                if (count <= 0) {
                    break;
                }
                raw.append(buffer, static_cast<std::size_t>(count));
            }
            if (auto request = parse_request(raw)) {
                const std::string response = route(*request);
                send(connection.get(), response.data(), static_cast<int>(response.size()), 0);
            }
        }
    });

    const std::string hello = http_get(port, "/hello");
    std::println("GET /hello -> {}", status_code(hello));
    std::println("body: {}", body_of(hello));

    const std::string missing = http_get(port, "/missing");
    std::println("GET /missing -> {}", status_code(missing));
    std::println("body: {}", body_of(missing));

    server.join();

    const bool ok = status_code(hello) == 200 && body_of(hello) == "Hello, HTTP!" &&
                    status_code(missing) == 404 && body_of(missing) == "not found";
    std::cout << "http server done\n";
    return ok ? 0 : 1;
}
