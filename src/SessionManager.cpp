#include "SessionManager.h"


void SessionManager::handleMessage(JSON message) {
    std::string message_as_string = message.dump();
    common::print("Received from socketio server:" + message_as_string);

    std::string sid = message.at("sid");
    JSON response;
    response["sid"] = sid;

    std::string stage = message.at("stage");
    if (common::stringCompare(stage, "prelobby")) {
        std::string task = message.at("task");

        if (common::stringCompare(task, "new_session")) {
            Session* new_session = createNewSession();
            std::string session_id = new_session->getID();
            std::string source_sid = message.at("sid");
            std::string player_id = addPlayerToSessionSID(session_id, source_sid);
            linkSIDToSessionID(source_sid, session_id);

            response["session_id"] = session_id;
            response["player_id"] = player_id;

            common::sendThroughSocketSID(response);
        }
        else if (common::stringCompare(task, "join_session")) {
            std::string session_id = message.at("session_id");
            std::string source_sid = message.at("sid");
            std::string player_id = addPlayerToSessionSID(session_id, source_sid);

            if (!ifSessionIDExists(session_id)) {
                common::print("Session ID not found: " + session_id);
                return;
            }

            linkSIDToSessionID(source_sid, session_id);

            response["session_id"] = session_id;
            response["player_id"] = player_id;
            common::sendThroughSocketSID(response);
        }
    }

    if (stage == "chat") {
        std::string session_id = message.at("session_id");
        Session* target_session = m_id_to_session.at(session_id);
        target_session->addToChat(message);
    }
}

void SessionManager::removeSID(std::string sid) {
    // If socket exists in the sessionmanager
    if (m_sid_to_sessionid.count(sid)) {
        std::string session_id = m_sid_to_sessionid.at(sid);

        //make new function in session that uses sid
        m_id_to_session.at(session_id)->removePlayerSID(sid);
    }
}

Session* SessionManager::createNewSession() {
    std::string new_id;
    do {
        new_id = generateSessionID();
    } while (ifSessionIDExists(new_id));

    m_id_to_session[new_id] = new Session(new_id);
    return m_id_to_session[new_id];
}

std::string SessionManager::addPlayerToSessionSID(std::string session_id, std::string sid) {
    Session* target_session = m_id_to_session[session_id];
    std::string player_id = target_session->addPlayerSID(sid);
    return player_id;
}

void SessionManager::linkSIDToSessionID(std::string sid, std::string id) {
    m_sid_to_sessionid[sid] = id;
}

bool SessionManager::ifSessionIDExists(std::string id) {
    if (m_id_to_session.find(id) == m_id_to_session.end())
        return false;
    return true;
}

std::string SessionManager::generateSessionID() {
    const string CHARACTERS = "abcdefghijklmnopqrstuv";

    random_device rd;
    mt19937 generator(rd());
    uniform_int_distribution<> distribution(0, CHARACTERS.size() - 1);

    string random_string = "s";
    for (int i = 0; i < ID_LENGTH - 1; ++i) {
        random_string
            += CHARACTERS[distribution(generator)];
    }

    return random_string;
}
