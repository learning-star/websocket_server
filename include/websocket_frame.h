#ifndef WEBSOCKET_FRAME_H
#define WEBSOCKET_FRAME_H

#include <vector>
#include <cstdint>

class WebSocketFrame {
public:
    WebSocketFrame();
    void encode(const std::vector<uint8_t>& data);
    std::vector<uint8_t> decode(const std::vector<uint8_t>& data);
    std::vector<uint8_t> getFrame() const;

private:
    std::vector<uint8_t> frame_;
};

#endif // WEBSOCKET_FRAME_H
