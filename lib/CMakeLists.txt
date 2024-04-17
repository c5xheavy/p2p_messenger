cmake_minimum_required(VERSION 3.10)

project(p2p_messenger CXX)
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

find_package(Boost 1.82.0 REQUIRED)
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
endif()

find_package(Catch2 REQUIRED)

find_library(OPENDHT_LIB opendht)

add_library(p2p_messenger_lib STATIC
    src/boost_json.cpp
    src/payload.h
    src/payload_serializer.h
    src/payload_serializer.cpp
    src/payload_deserializer.h
    src/payload_deserializer.cpp
    src/message.h
    src/message_serializer.h
    src/message_serializer.cpp
    src/message_deserializer.h
    src/message_deserializer.cpp
    src/login_hasher.h
    src/login_hasher.cpp
    src/ip_resolver.h
    src/dht_ip_resolver.h
    src/dht_ip_resolver.cpp
    src/static_file_ip_resolver.h
    src/static_file_ip_resolver.cpp
)

target_link_libraries(p2p_messenger_lib PRIVATE Threads::Threads)

add_executable(cli_client
    src/cli_client.cpp
)

target_link_libraries(cli_client PRIVATE p2p_messenger_lib PRIVATE "${OPENDHT_LIB}" PRIVATE Threads::Threads)

add_executable(p2p_messenger_tests
    tests/payload_utils.h
    tests/payload_utils.cpp
    tests/payload_serializer_tests.cpp
    tests/payload_deserializer_tests.cpp
    tests/message_utils.h
    tests/message_utils.cpp
    tests/message_serializer_deserializer_tests.cpp
    tests/static_file_ip_resolver_tests.cpp
)

target_link_libraries(p2p_messenger_tests PRIVATE p2p_messenger_lib PRIVATE Catch2::Catch2WithMain)
