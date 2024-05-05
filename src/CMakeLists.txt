cmake_minimum_required(VERSION 3.16.3)
project(shengji-backend)

set(SOURCE_FILES
    main.cpp
    Deck.cpp
    Card.cpp
    SocketServer.cpp
    AcceptedSocket.cpp
    SessionManager.cpp
    Session.cpp
    Chat.cpp
    common.cpp
    Player.cpp)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)


# install(TARGETS ${PROJECT_NAME} DESTINATION bin)