cmake_minimum_required(VERSION 3.10)

project(low_latency_media_relay)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

add_compile_options(
    -Wall
    -Wextra
    -Wpedantic
    -Wconversion
    -Wsign-conversion
)

include_directories(${PROJECT_SOURCE_DIR}/include)

enable_testing()

# -------------------------
# Source files
# -------------------------
file(GLOB_RECURSE BASE_SRC CONFIGURE_DEPENDS src/base/*.cpp)
file(GLOB_RECURSE NET_SRC CONFIGURE_DEPENDS src/net/*.cpp)
file(GLOB_RECURSE STREAM_SRC CONFIGURE_DEPENDS src/stream/*.cpp)
file(GLOB_RECURSE PROTOCOL_SRC CONFIGURE_DEPENDS src/protocol/*.cpp)
file(GLOB_RECURSE APP_SRC CONFIGURE_DEPENDS src/app/*.cpp)

set(SOURCES
    ${BASE_SRC}
    ${NET_SRC}
    ${STREAM_SRC}
    ${PROTOCOL_SRC}
    ${APP_SRC}
)

# -------------------------
# Main server executable
# -------------------------
add_executable(stream_relay_server
    main.cpp
    ${SOURCES}
)

find_package(Threads REQUIRED)
target_link_libraries(stream_relay_server PRIVATE Threads::Threads)

target_compile_definitions(stream_relay_server PRIVATE
    $<$<CONFIG:Debug>:DEBUG>
)

set_target_properties(stream_relay_server PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin
)

add_executable(publisher_client
    examples/publisher_client.cpp
    ${BASE_SRC}
    ${NET_SRC}
    ${PROTOCOL_SRC}
)

add_executable(subscriber_client
    examples/subscriber_client.cpp
    ${BASE_SRC}
    ${NET_SRC}
    ${PROTOCOL_SRC}
)

target_link_libraries(publisher_client PRIVATE Threads::Threads)
target_link_libraries(subscriber_client PRIVATE Threads::Threads)

set_target_properties(publisher_client subscriber_client PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin
)

# -------------------------
# Catch2 tests
# -------------------------
find_package(Catch2 3 REQUIRED)

add_executable(stream_tests
    tests/Buffer_test.cpp
    tests/Codec_test.cpp
    tests/StreamManager_test.cpp
    tests/BackpressurePolicy_test.cpp
    ${SOURCES}
)

target_link_libraries(stream_tests
    PRIVATE
    Catch2::Catch2WithMain
    Threads::Threads
)

target_compile_definitions(stream_tests PRIVATE
    $<$<CONFIG:Debug>:DEBUG>
)

include(CTest)
include(Catch)

catch_discover_tests(stream_tests)
