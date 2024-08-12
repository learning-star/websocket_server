#include "websocket_server.h"

int main() {
    WebSocketServer server(8080, 4);
    server.run();
    return 0;
}
