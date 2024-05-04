#include "SessionManager.h"

#include <random>


SessionManager::SessionManager() {

}

Session* SessionManager::createNewSession() {
    std::string new_id;
    do {
        new_id = generateSessionID();
    } while (ifSessionIDExists(new_id));

    m_id_to_session[new_id] = new Session(new_id);
    return m_id_to_session[new_id];
}

int SessionManager::addPlayerToSession(std::string session_id, int player) {
    Session* target_session = m_id_to_session[session_id];
    int player_id = target_session->addPlayer(player);
    return player_id;
}

bool SessionManager::ifSessionIDExists(std::string id) {
    if (m_id_to_session.find(id) == m_id_to_session.end())
        return false;
    return true;
}

std::string SessionManager::generateSessionID() {
    // Define the list of possible characters
    const string CHARACTERS
        = "abcdefghijklmnopqrstuv";

    // Create a random number generator
    random_device rd;
    mt19937 generator(rd());

    // Create a distribution to uniformly select from all characters
    uniform_int_distribution<> distribution(
        0, CHARACTERS.size() - 1);

    // Generate the random string
    string random_string;
    for (int i = 0; i < ID_LENGTH; ++i) {
        random_string
            += CHARACTERS[distribution(generator)];
    }

    return random_string;
}

void SessionManager::receiveJSON(JSON message) {
    std::string stage = message.at("stage");

    if (common::stringCompare(stage, "prelobby")) {
        std::string task = message.at("task");

        if (common::stringCompare(task, "new_session")) {
            Session* new_session = createNewSession();
            std::string session_id = new_session->getID();
            int source_fd = message.at("source_fd");
            int player_id = addPlayerToSession(session_id, source_fd);
            linkSocketToSessionID(source_fd, session_id);

            JSON response;
            response["session_id"] = session_id;
            response["player_id"] = player_id;
            common::sendThroughSocket(source_fd, response);
        }
        else if (common::stringCompare(task, "join_session")) {
            std::string session_id = message.at("session_id");
            int source_fd = message.at("source_fd");
            int player_id = addPlayerToSession(session_id, source_fd);
            linkSocketToSessionID(source_fd, session_id);

            JSON response;
            response["session_id"] = session_id;
            response["player_id"] = player_id;
            common::sendThroughSocket(source_fd, response);
        }

    }

    if (stage == "chat") {
        std::string session_id = message.at("session_id");
        Session* target_session = m_id_to_session.at(session_id);
        target_session->addToChat(message);
    }

}

void SessionManager::linkSocketToSessionID(int socket, std::string id) {
    m_socket_to_sessionid[socket] = id;
}

void SessionManager::removeSocket(int socket) {
    std::string session_id = m_socket_to_sessionid.at(socket);
    m_id_to_session.at(session_id)->removePlayer(socket);
}