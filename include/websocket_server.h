#ifndef WEBSOCKET_SERVER_H
#define WEBSOCKET_SERVER_H

#include "websocket_connection.h"
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>

class WebSocketServer {
public:
    WebSocketServer(int port, int thread_pool_size);
    void run();

private:
    int port_;
    int server_socket_;
    std::vector<std::thread> thread_pool_;
    std::queue<int> connection_queue_;
    std::mutex queue_mutex_;
    std::condition_variable condition_;
    void acceptConnections();
    void handleConnections();
};

#endif // WEBSOCKET_SERVER_H
