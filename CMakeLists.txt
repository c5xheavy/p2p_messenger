cmake_minimum_required(VERSION 3.5)

project(p2p_messenger VERSION 0.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

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
    src/message_receiver.h
    src/message_receiver.cpp
    src/message_sender.h
    src/message_sender.cpp
    src/message_serializer.h
    src/message_serializer.cpp
    src/message_deserializer.h
    src/message_deserializer.cpp
    src/dht_ip_resolver.h
    src/dht_ip_resolver.cpp
)

target_link_libraries(p2p_messenger_lib PRIVATE Threads::Threads)

if(MSVC)
  target_compile_options(p2p_messenger_lib PRIVATE /W4 /WX)
else()
  target_compile_options(p2p_messenger_lib PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

add_executable(cli_client
    src/cli_client.cpp
)

target_link_libraries(cli_client PRIVATE p2p_messenger_lib PRIVATE "${OPENDHT_LIB}" PRIVATE Threads::Threads)

if(MSVC)
  target_compile_options(cli_client PRIVATE /W4 /WX)
else()
  target_compile_options(cli_client PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

add_executable(p2p_messenger_tests
    tests/payload_utils.h
    tests/payload_utils.cpp
    tests/payload_serializer_deserializer_tests.cpp
    tests/message_utils.h
    tests/message_utils.cpp
    tests/message_serializer_deserializer_tests.cpp
)

target_link_libraries(p2p_messenger_tests PRIVATE p2p_messenger_lib PRIVATE Catch2::Catch2WithMain)

if(MSVC)
  target_compile_options(p2p_messenger_tests PRIVATE /W4 /WX)
else()
  target_compile_options(p2p_messenger_tests PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

set(PROJECT_SOURCES
        src/main.cpp
        src/p2pmessenger.cpp
        src/p2pmessenger.h
        src/p2pmessenger.ui
        src/chatpage.cpp
        src/chatpage.h
        src/chatpage.ui
        src/loginpage.cpp
        src/loginpage.h
        src/loginpage.ui
        src/p2p_messenger_impl.h
        src/p2p_messenger_impl.cpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(p2p_messenger
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET p2p_messenger APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(p2p_messenger SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(p2p_messenger
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(p2p_messenger PRIVATE Qt${QT_VERSION_MAJOR}::Widgets PRIVATE p2p_messenger_lib PRIVATE "${OPENDHT_LIB}" PRIVATE Threads::Threads)

if(MSVC)
  target_compile_options(p2p_messenger PRIVATE /W4 /WX)
else()
  target_compile_options(p2p_messenger PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.p2p_messenger)
endif()
set_target_properties(p2p_messenger PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS p2p_messenger
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(p2p_messenger)
endif()
