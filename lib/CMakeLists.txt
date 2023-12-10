cmake_minimum_required(VERSION 3.10)

project(messenger CXX)
set(CMAKE_CXX_STANDARD 20)

#find_package(Boost 1.82.0 COMPONENTS json REQUIRED)
find_package(Boost 1.82.0 REQUIRED)

find_package(Catch2 REQUIRED)

add_executable(messenger
    src/main.cpp
    src/boost_json.cpp
    src/message.h
    src/message_serializer.h
    src/message_serializer.cpp
    src/message_deserializer.h
    src/message_deserializer.cpp
    src/datagram.h
    src/datagram_serializer.h
    src/datagram_serializer.cpp
    src/datagram_deserializer.h
    src/datagram_deserializer.cpp
)

add_executable(sender
    src/sender.cpp
    src/boost_json.cpp
    src/message.h
    src/message_serializer.h
    src/message_serializer.cpp
    src/message_deserializer.h
    src/message_deserializer.cpp
    src/datagram.h
    src/datagram_serializer.h
    src/datagram_serializer.cpp
    src/datagram_deserializer.h
    src/datagram_deserializer.cpp
)

add_executable(recipient
    src/recipient.cpp
    src/boost_json.cpp
    src/message.h
    src/message_serializer.h
    src/message_serializer.cpp
    src/message_deserializer.h
    src/message_deserializer.cpp
    src/datagram.h
    src/datagram_serializer.h
    src/datagram_serializer.cpp
    src/datagram_deserializer.h
    src/datagram_deserializer.cpp
)

add_executable(messenger_tests
    src/boost_json.cpp
    src/message.h
    src/message_serializer.h
    src/message_serializer.cpp
    src/message_deserializer.h
    src/message_deserializer.cpp
    tests/message_utils.h
    tests/message_utils.cpp
    tests/message_serializer_tests.cpp
    tests/message_deserializer_tests.cpp
    src/datagram.h
    src/datagram_serializer.h
    src/datagram_serializer.cpp
    src/datagram_deserializer.h
    src/datagram_deserializer.cpp
    tests/datagram_utils.h
    tests/datagram_utils.cpp
    tests/datagram_serializer_deserializer_tests.cpp
)

#target_link_libraries(messenger ${Boost_LIBRARIES})

target_link_libraries(messenger_tests PRIVATE Catch2::Catch2WithMain)
