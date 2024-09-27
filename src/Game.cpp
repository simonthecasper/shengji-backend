#include "Game.h"

Game::Game() {

}


Connection* Game::getConnectionFromPlayerID(std::string player_id) {
    for (auto const& curr_conn : m_connection_list) {
        if (common::stringCompare(curr_conn->getID(), player_id)) {
            return curr_conn;
        }
    }
    return nullptr;
}
