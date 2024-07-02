#include "Connection.h"


Connection::Connection(Player* player, std::string sid) {
    m_player = player;
    m_sid = sid;
    m_state = user_connected;
}

Player* Connection::getPlayer() {
    return m_player;
}

std::string Connection::getPlayerID() {
    return m_player->getID();
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

std::string Connection::getUsername() {
    return m_player->getUsername();
}

int Connection::sendMessageToUser(JSON message) {
    if (m_state == user_connected) {
        message["sid"] = m_sid;
        return common::sendToMiddleman(message);
    }

    //TODO: Add user_disconnected_case
    return 0;
}

void Connection::setTeam(std::string team) {
    m_player->setTeam(team);
}

std::string Connection::getTeam() {
    return m_player->getTeam();
}