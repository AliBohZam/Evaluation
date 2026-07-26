cmake_minimum_required(VERSION 3.20)

project(PingPongIPC VERSION 1.0 LANGUAGES CXX)

option(ENABLE_LOGGING "Enable logging" ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Common warnings
set(COMMON_WARNINGS
    -Wall
    -Wextra
    -Wpedantic
)

# Optimization flags
set(COMMON_OPTIMIZATIONS
    -O3
)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(
        ${COMMON_WARNINGS}
        ${COMMON_OPTIMIZATIONS}
    )
endif()

if(ENABLE_LOGGING)
    add_compile_definitions(ENABLE_LOGGING)
endif()

include_directories(include)

add_library(logger
    src/logger.cpp
)

target_include_directories(logger PUBLIC include)

add_library(ipc
    src/ipc.cpp
)

target_include_directories(ipc PUBLIC include)

# Linux requires librt for shm_open on some systems.
if(UNIX AND NOT APPLE)
    target_link_libraries(ipc PUBLIC rt pthread)
else()
    target_link_libraries(ipc PUBLIC pthread)
endif()

add_executable(initiator
    initiator/main.cpp
)

target_link_libraries(initiator
    PRIVATE
    ipc
    logger
)

add_executable(receiver
    receiver/main.cpp
)

target_link_libraries(receiver
    PRIVATE
    ipc
    logger
)
