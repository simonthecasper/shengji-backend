#include "Game.h"

Game::Game() {

}


bool Game::verifyIncomingMessageIsFromExpectedPlayer(JSON message) {
    return common::stringCompare(m_expected_player->getID(), message.at("player_id"));
}


void Game::setLordPlayer(Player* lord_player) {
    m_lord_player = lord_player;
}

void Game::setTrickStarterPlayer(Player* trick_starter) {
    m_trick_starter = trick_starter;
}

void Game::incrementExpectedPlayer() {
    m_expected_player_itr++;
    if ((m_expected_player_itr) == m_player_order.end()) {
        m_expected_player_itr = m_player_order.begin();
    }
    m_expected_player = *m_expected_player_itr;
}

void Game::setExpectedPlayer(Player* expected_player) {
    while (m_expected_player != expected_player) {
        incrementExpectedPlayer();
    }
}

void Game::setWinningPlayer(Player* winning_player) {
    m_winning_player = winning_player;
}

void Game::setStrongSuit(std::string strong_suit) {
    m_strong_suit = strong_suit;
}
