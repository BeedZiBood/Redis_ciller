# server/CMakeLists.txt

set(SERVER_SOURCES
        main.cpp
        server.cpp
        server.h
        common.h
        common.cpp
)

add_executable(server ${SERVER_SOURCES})

target_link_libraries(server PRIVATE Boost::system Boost::asio Boost::thread ws2_32 wsock32 Boost::log Boost::log_setup)
