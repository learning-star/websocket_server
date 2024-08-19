#ifndef WEBSOCKET_CONNECTION_H
#define WEBSOCKET_CONNECTION_H

#include "websocket_frame.h"
#include "http_request.h"
#include "http_response.h"
#include <netinet/in.h>

class WebSocketConnection {
public:
    WebSocketConnection(int client_socket);
    void handleConnection();

private:
    int client_socket_;
    WebSocketFrame frame_;
    void handleHttpRequest(const HttpRequest& request);
    void handleWebSocketMessage(char* buffer, ssize_t bytes_read);
    void sendTextMessage(const std::string& message);
    void sendPongFrame();

    std::string generateWebSocketAcceptKey(const std::string& key);
};

#endif // WEBSOCKET_CONNECTION_H
