#include "ShengJi.h"

ShengJi::ShengJi() {

}


void ShengJi::handleMessage(JSON message) {
    common::print("In game handler...");

    std::string stage = message.at("stage");
    std::string task = message.at("task");

    if (common::stringCompare(task, "set_team")) {
        std::string player_id = message.at("player_id");
        std::string team = message.at("team");
        updatePlayerTeam(player_id, team);
    }


}

bool ShengJi::isMessageFromExpectedPlayer(JSON message) {
    return common::stringCompare(m_expected_player->getPlayerID(), message.at("player_id"));
}


void ShengJi::setLordPlayer(Connection* lord_player) {
    m_lord_player = lord_player;
}

void ShengJi::setTrickStarterPlayer(Connection* trick_starter) {
    m_trick_starter = trick_starter;
}

void ShengJi::incrementExpectedPlayer() {
    m_expected_player_itr++;
    if ((m_expected_player_itr) == m_player_order.end()) {
        m_expected_player_itr = m_player_order.begin();
    }
    m_expected_player = *m_expected_player_itr;
}

void ShengJi::setExpectedPlayer(Connection* expected_player) {
    while (m_expected_player != expected_player) {
        incrementExpectedPlayer();
    }
}

void ShengJi::setWinningPlayer(Connection* winning_player) {
    m_winning_player = winning_player;
}

void ShengJi::setStrongSuit(std::string strong_suit) {
    m_strong_suit = strong_suit;
}
