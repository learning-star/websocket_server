#include "websocket_connection.h"
#include <openssl/sha.h>
#include <unistd.h>
#include <iostream>
#include <vector>

WebSocketConnection::WebSocketConnection(int client_socket)
    : client_socket_(client_socket) {}

void WebSocketConnection::handleConnection() {
    char buffer[1024] = {0};
    ssize_t bytes_read = recv(client_socket_, buffer, sizeof(buffer), 0);
    if (bytes_read <= 0) {
        close(client_socket_);
        return;
    }

    HttpRequest request(std::string(buffer, bytes_read));
    handleHttpRequest(request);

    while (true) {
        bytes_read = recv(client_socket_, buffer, sizeof(buffer), 0);
        if (bytes_read <= 0) {
            close(client_socket_);
            break;
        }
        handleWebSocketMessage(buffer, bytes_read);
    }
}

void WebSocketConnection::handleHttpRequest(const HttpRequest& request) {
    std::string websocket_key = request.getHeader("Sec-WebSocket-Key");
    std::string accept_key = generateWebSocketAcceptKey(websocket_key);

    HttpResponse response;
    response.setStatus("101 Switching Protocols");
    response.setHeader("Upgrade", "websocket");
    response.setHeader("Connection", "Upgrade");
    response.setHeader("Sec-WebSocket-Accept", accept_key);

    std::string response_str = response.getResponse();
    send(client_socket_, response_str.c_str(), response_str.size(), 0);
}

void WebSocketConnection::handleWebSocketMessage(char* buffer, ssize_t bytes_read) {
    // 解析并处理 WebSocket 帧
    uint8_t* data = reinterpret_cast<uint8_t*>(buffer);

    // 第一个字节包含 FIN 位和操作码
    bool fin = (data[0] & 0x80) != 0;
    uint8_t opcode = data[0] & 0x0F;

    // 第二个字节包含掩码位和负载长度
    bool isMasked = (data[1] & 0x80) != 0;
    size_t payloadLength = data[1] & 0x7F;

    size_t offset = 2; // 跳过前两个字节

    // 根据负载长度字节数调整 offset
    if (payloadLength == 126) {
        payloadLength = (data[2] << 8) | data[3];
        offset += 2;
    } else if (payloadLength == 127) {
        payloadLength = 0;
        for (int i = 0; i < 8; i++) {
            payloadLength = (payloadLength << 8) | data[2 + i];
        }
        offset += 8;
    }

    uint8_t* maskingKey = nullptr;
    if (isMasked) {
        maskingKey = &data[offset];
        offset += 4; // 掩码键的长度为 4 字节
    }

    uint8_t* payloadData = &data[offset];

    // 如果数据被掩码处理，进行解码
    if (isMasked) {
        for (size_t i = 0; i < payloadLength; i++) {
            payloadData[i] ^= maskingKey[i % 4];
        }
    }

    // 处理不同的 WebSocket 操作码
    switch (opcode) {
        case 0x1: { // 文本帧
            std::string message(payloadData, payloadData + payloadLength);
            std::cout << "Received text message: " << message << std::endl;
            // 回显收到的消息
            sendTextMessage(message);
            break;
        }
        case 0x2: { // 二进制帧
            std::cout << "Received binary message." << std::endl;
            // 处理二进制数据
            break;
        }
        case 0x8: { // 关闭连接帧
            std::cout << "Received close frame." << std::endl;
            close(client_socket_);
            break;
        }
        case 0x9: { // Ping 帧
            std::cout << "Received ping frame." << std::endl;
            sendPongFrame();
            break;
        }
        case 0xA: { // Pong 帧
            std::cout << "Received pong frame." << std::endl;
            break;
        }
        default:
            std::cerr << "Received unknown opcode: " << static_cast<int>(opcode) << std::endl;
            break;
    }
}

void WebSocketConnection::sendTextMessage(const std::string& message) {
    std::vector<uint8_t> encodedMessage(message.begin(), message.end());
    frame_.encode(encodedMessage);
    std::vector<uint8_t> response = frame_.getFrame();
    send(client_socket_, response.data(), response.size(), 0);
}

void WebSocketConnection::sendPongFrame() {
    uint8_t pongFrame[2] = {0x8A, 0x00}; // FIN = 1, Opcode = 0xA (Pong)
    send(client_socket_, pongFrame, 2, 0);
}


std::string WebSocketConnection::generateWebSocketAcceptKey(const std::string& key) {
    std::string magic_string = key + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA1(reinterpret_cast<const unsigned char*>(magic_string.c_str()), magic_string.size(), hash);
    
    // Base64 encode
    static const char* b64_table = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::string result;
    int i = 0;
    for (; i + 2 < SHA_DIGEST_LENGTH; i += 3) {
        result.push_back(b64_table[hash[i] >> 2]);
        result.push_back(b64_table[((hash[i] & 0x03) << 4) | (hash[i + 1] >> 4)]);
        result.push_back(b64_table[((hash[i + 1] & 0x0F) << 2) | (hash[i + 2] >> 6)]);
        result.push_back(b64_table[hash[i + 2] & 0x3F]);
    }
    if (i < SHA_DIGEST_LENGTH) {
        result.push_back(b64_table[hash[i] >> 2]);
        if (i + 1 < SHA_DIGEST_LENGTH) {
            result.push_back(b64_table[((hash[i] & 0x03) << 4) | (hash[i + 1] >> 4)]);
            result.push_back(b64_table[(hash[i + 1] & 0x0F) << 2]);
            result.push_back('=');
        } else {
            result.push_back(b64_table[(hash[i] & 0x03) << 4]);
            result.push_back('=');
            result.push_back('=');
        }
    }
    return result;
}
