#include "Game.h"

Game::Game() {

}


void Game::updatePlayerTeam(std::string player_id, std::string team) {
    Connection* target_connection = getConnectionFromPlayerID(player_id);

    if (target_connection == nullptr) {
        common::print("No connection was found with the provided player_id");
        return;
    }

    target_connection->setTeam(team);
    S2CMessages::sendBroadcastTeamUpdate(m_connection_list);
}


Connection* Game::getConnectionFromPlayerID(std::string player_id) {
    for (auto const& curr_conn : m_connection_list) {
        if (common::stringCompare(curr_conn->getPlayerID(), player_id)) {
            return curr_conn;
        }
    }
    return nullptr;
}
