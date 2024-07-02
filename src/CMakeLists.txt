cmake_minimum_required(VERSION 3.16.3)
project(shengji-backend)

set(SOURCE_FILES
    main.cpp
    SocketServer.cpp
    AcceptedSocket.cpp
    SessionManager.cpp
    Game.cpp
    Session.cpp
    Connection.cpp
    Player.cpp
    Play.cpp
    Deck.cpp
    Card.cpp
    Chat.cpp
    common.cpp
    S2CMessages.cpp)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)


# install(TARGETS ${PROJECT_NAME} DESTINATION bin)