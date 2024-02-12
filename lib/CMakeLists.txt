cmake_minimum_required(VERSION 3.10)

project(messenger CXX)
set(CMAKE_CXX_STANDARD 20)

find_package(Boost 1.82.0 REQUIRED)

find_package(Catch2 REQUIRED)

add_executable(messenger
    src/main.cpp
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

add_executable(source
    src/source.cpp
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

add_executable(destination
    src/destination.cpp
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

add_executable(messenger_tests
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
    tests/payload_utils.h
    tests/payload_utils.cpp
    tests/payload_serializer_tests.cpp
    tests/payload_deserializer_tests.cpp
    tests/message_utils.h
    tests/message_utils.cpp
    tests/message_serializer_deserializer_tests.cpp
    tests/static_file_ip_resolver_tests.cpp
)

target_link_libraries(messenger_tests PRIVATE Catch2::Catch2WithMain)
