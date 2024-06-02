cmake_minimum_required(VERSION 3.14)
project(Redis_killer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
        boost
        GIT_REPOSITORY https://github.com/boostorg/boost.git
        GIT_TAG develop
)
FetchContent_MakeAvailable(boost)

add_subdirectory(server)

add_subdirectory(client)