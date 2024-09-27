#include "Connection.h"


Connection::Connection(std::string id, std::string username, std::string sid) {
    m_id = id;
    m_username = username;
    m_sid = sid;
    m_state = user_connected;
    m_color = "00B6FF";
    updatePlayerAttributes();
}

int Connection::setSID(std::string sid) {
    if (m_state == user_connected) { return -1; }

    else {
        m_sid = sid;
        m_state = user_connected;
        return 0;
    }
}

std::string Connection::getSID() {
    return m_sid;
}

int Connection::disconnectSID() {
    if (m_state == user_disconnected) { return -1; }

    else {
        m_state = user_disconnected;
        m_sid = "";
        return 0;
    }
}

int Connection::sendMessageToUser(JSON message) {
    if (m_state == user_connected) {
        message["sid"] = m_sid;
        return common::sendToMiddleman(message);
    }

    //TODO: Add user_disconnected_case
    return 0;
}