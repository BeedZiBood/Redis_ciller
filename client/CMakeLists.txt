# client/CMakeLists.txt

set(CLIENT_SOURCES client.cpp)

add_executable(client ${CLIENT_SOURCES})

target_link_libraries(client PRIVATE Boost::system Boost::asio Boost::thread ws2_32)
