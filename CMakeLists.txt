cmake_minimum_required(VERSION 3.10)

project(WebSocketServer)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 设置编译输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
# 添加头文件路径
include_directories(${CMAKE_SOURCE_DIR}/include)

# 设置构建类型为调试
set(CMAKE_BUILD_TYPE Debug)

# 确保调试信息被包含
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g")

add_executable(websocket_server
    src/websocket_frame.cpp
    src/http_request.cpp
    src/http_response.cpp
    src/websocket_connection.cpp
    src/websocket_server.cpp
    src/thread_pool.cpp
    main.cpp
)

target_link_libraries(websocket_server pthread ssl crypto)
