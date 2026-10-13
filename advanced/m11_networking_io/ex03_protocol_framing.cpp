// m11_networking_io/ex03_protocol_framing — freyming soobshcheniy (length-prefix).
// Standart: C++23. Zapusk: ./run.ps1 m11_networking_io/ex03_protocol_framing -Extra '-lws2_32'
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

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <optional>
#include <print>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

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

std::vector<std::uint8_t> encode_frame(const std::string& payload) {
    const auto length = static_cast<std::uint32_t>(payload.size());
    std::vector<std::uint8_t> frame;
    frame.push_back(static_cast<std::uint8_t>((length >> 24) & 0xFF));
    frame.push_back(static_cast<std::uint8_t>((length >> 16) & 0xFF));
    frame.push_back(static_cast<std::uint8_t>((length >> 8) & 0xFF));
    frame.push_back(static_cast<std::uint8_t>(length & 0xFF));
    frame.insert(frame.end(), payload.begin(), payload.end());
    return frame;
}

class FrameDecoder {
public:
    void feed(const std::uint8_t* data, std::size_t count) {
        buffer_.insert(buffer_.end(), data, data + count);
    }

    std::optional<std::string> next() {
        if (buffer_.size() < 4) {
            return std::nullopt;
        }
        const std::uint32_t length = (static_cast<std::uint32_t>(buffer_[0]) << 24) |
                                     (static_cast<std::uint32_t>(buffer_[1]) << 16) |
                                     (static_cast<std::uint32_t>(buffer_[2]) << 8) |
                                     static_cast<std::uint32_t>(buffer_[3]);
        if (buffer_.size() < 4 + length) {
            return std::nullopt;
        }
        std::string payload(reinterpret_cast<const char*>(buffer_.data() + 4), length);
        buffer_.erase(buffer_.begin(), buffer_.begin() + static_cast<std::ptrdiff_t>(4 + length));
        return payload;
    }

    std::size_t buffered() const { return buffer_.size(); }

private:
    std::vector<std::uint8_t> buffer_;
};

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

    const std::vector<std::uint8_t> hello = encode_frame("hello");
    const std::vector<std::uint8_t> world = encode_frame("world");
    std::println("encoded(\"hello\") = {} bytes (4 zagolovok + 5 payload)", hello.size());
    std::println("encoded(\"world\") = {} bytes", world.size());

    std::vector<std::uint8_t> stream = encode_frame("hello");
    const std::vector<std::uint8_t> second = encode_frame("world");
    stream.insert(stream.end(), second.begin(), second.end());

    FrameDecoder decoder;
    decoder.feed(stream.data(), 3);
    const bool early_ready = decoder.next().has_value();
    std::println("after 3 bytes: frame ready = {}", early_ready);
    decoder.feed(stream.data() + 3, stream.size() - 3);

    std::vector<std::string> pure_frames;
    while (auto frame = decoder.next()) {
        pure_frames.push_back(*frame);
    }
    std::println("decoded (pure): {}, {}", pure_frames[0], pure_frames[1]);

    std::uint16_t port = 0;
    Socket listener = listen_loopback(port);
    std::vector<std::string> transport_frames;

    std::jthread server([&listener, &transport_frames] {
        Socket connection(accept(listener.get(), nullptr, nullptr));
        FrameDecoder server_decoder;
        char buffer[64] = {};
        while (transport_frames.size() < 2) {
            const int count = recv(connection.get(), buffer, sizeof(buffer), 0);
            if (count <= 0) {
                break;
            }
            server_decoder.feed(reinterpret_cast<const std::uint8_t*>(buffer),
                                static_cast<std::size_t>(count));
            while (auto frame = server_decoder.next()) {
                transport_frames.push_back(*frame);
            }
        }
    });

    {
        Socket client = connect_loopback(port);
        const std::vector<std::uint8_t> outgoing = [&] {
            std::vector<std::uint8_t> bytes = encode_frame("hello");
            const std::vector<std::uint8_t> second_frame = encode_frame("world");
            bytes.insert(bytes.end(), second_frame.begin(), second_frame.end());
            return bytes;
        }();
        send(client.get(), reinterpret_cast<const char*>(outgoing.data()),
             static_cast<int>(outgoing.size()), 0);
    }
    server.join();

    std::println("transport: received {} frames", transport_frames.size());
    std::println("  {}", transport_frames[0]);
    std::println("  {}", transport_frames[1]);

    const bool ok = !early_ready && pure_frames.size() == 2 && pure_frames[0] == "hello" &&
                    pure_frames[1] == "world" && transport_frames.size() == 2 &&
                    transport_frames[0] == "hello" && transport_frames[1] == "world";
    std::cout << "protocol framing done\n";
    return ok ? 0 : 1;
}
