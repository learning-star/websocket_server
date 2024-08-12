#include "websocket_frame.h"

WebSocketFrame::WebSocketFrame() : frame_() {}

void WebSocketFrame::encode(const std::vector<uint8_t>& data) {
    // 简单实现，未处理掩码和控制帧
    frame_ = {};
    frame_.push_back(0x81);  // Text frame
    if (data.size() <= 125) {
        frame_.push_back(static_cast<uint8_t>(data.size()));
    } else if (data.size() <= 65535) {
        frame_.push_back(126);
        frame_.push_back((data.size() >> 8) & 0xFF);
        frame_.push_back(data.size() & 0xFF);
    } else {
        frame_.push_back(127);
        for (int i = 7; i >= 0; --i) {
            frame_.push_back((data.size() >> (8 * i)) & 0xFF);
        }
    }
    frame_.insert(frame_.end(), data.begin(), data.end());
}

std::vector<uint8_t> WebSocketFrame::decode(const std::vector<uint8_t>& data) {
    // 简单实现，未处理掩码和控制帧
    return std::vector<uint8_t>(data.begin() + 2, data.end());
}

std::vector<uint8_t> WebSocketFrame::getFrame() const {
    return frame_;
}
