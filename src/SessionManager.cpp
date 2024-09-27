#include "SessionManager.h"


SessionManager::SessionManager() {

}

void SessionManager::handleMessage(JSON message) {
    common::print("In SessionManager handler...");
    std::string message_as_string = message.dump();
    common::print("Received from socketio server:" + message_as_string);

    std::string sid = message.at("sid");
    // std::string stage = message.contains("stage") ? message.at("stage") : "all";
    std::string task = message.at("task");


    if (common::stringCompare(task, "new_session")) {
        common::print("Creating new session...");
        Session* new_session = createNewSession();
        std::string session_id = new_session->getID();

        new_session->handleMessage(message);
    }

    else if (common::stringCompare(task, "join_session")) {
        std::string session_id = message.at("session_id");

        if (doesSessionIDExist(session_id)) {
            linkSIDToSessionID(sid, session_id);
            Session* target_session = m_id_to_session.at(session_id);
            target_session->handleMessage(message);
        }
        else {
            S2CMessages::sendJoinSessionNotFound(sid);
        }
    }

    else {
        std::string session_id = message.at("session_id");
        Session* target_session = m_id_to_session.at(session_id);
        target_session->handleMessage(message);
    }
}

Session* SessionManager::createNewSession() {
    std::string new_id = generateSessionID();
    m_id_to_session[new_id] = new Session(new_id);
    return m_id_to_session[new_id];
}

void SessionManager::linkSIDToSessionID(std::string sid, std::string id) {
    m_sid_to_sessionid[sid] = id;
}

bool SessionManager::doesSessionIDExist(std::string id) const {
    return (m_id_to_session.find(id) != m_id_to_session.end());
}

void SessionManager::removeSID(std::string sid) {
    // If SID exists in the sessionmanager
    if (m_sid_to_sessionid.count(sid)) {
        std::string session_id = m_sid_to_sessionid.at(sid);

        //make new function in session that uses sid
        m_id_to_session.at(session_id)->removeSID(sid);
    }
}

std::string SessionManager::generateSessionID() {
    const std::string CHARACTERS = "bcdfghjklmpqrtv";

    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<> distribution(0, CHARACTERS.size() - 1);


    std::string random_string = "s";

    do {
        for (int i = 0; i < ID_LENGTH - 1; ++i) {
            random_string
                += CHARACTERS[distribution(generator)];
        }
    } while (doesSessionIDExist(random_string));
    return random_string;
}
