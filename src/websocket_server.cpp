#include "websocket_server.h"
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>

WebSocketServer::WebSocketServer(int port, int thread_pool_size)
    : port_(port), server_socket_(0), thread_pool_(thread_pool_size) {}

void WebSocketServer::run() {
    struct sockaddr_in address;
    int opt = 1;

    if ((server_socket_ = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_socket_, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt failed");
        close(server_socket_);
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port_);

    if (bind(server_socket_, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        close(server_socket_);
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket_, 3) < 0) {
        perror("listen failed");
        close(server_socket_);
        exit(EXIT_FAILURE);
    }

    std::thread(&WebSocketServer::acceptConnections, this).detach();

    for (size_t i = 0; i < thread_pool_.size(); ++i) {
        thread_pool_[i] = std::thread(&WebSocketServer::handleConnections, this);
    }

    for (auto &thread : thread_pool_) {
        if (thread.joinable()) {
            thread.join();
        }
    }
}

void WebSocketServer::acceptConnections() {
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    while (true) {
        int client_socket = accept(server_socket_, (struct sockaddr *)&address, (socklen_t*)&addrlen);
        if (client_socket < 0) {
            perror("accept failed");
            continue;
        }

        {
            std::lock_guard<std::mutex> lock(queue_mutex_);
            connection_queue_.push(client_socket);
        }
        condition_.notify_one();
    }
}

void WebSocketServer::handleConnections() {
    while (true) {
        int client_socket;
        {
            std::unique_lock<std::mutex> lock(queue_mutex_);
            condition_.wait(lock, [this]{ return !connection_queue_.empty(); });
            client_socket = connection_queue_.front();
            connection_queue_.pop();
        }
        WebSocketConnection connection(client_socket);
        connection.handleConnection();
    }
}
