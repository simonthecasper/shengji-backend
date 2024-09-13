#include "ShengJi.h"

ShengJi::ShengJi() {

}


void ShengJi::createDeck(int player_count, int deck_count) {

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


    else if (common::stringCompare(task, "start_game")) {
        startGameWithSettings(message);
    }

}

void ShengJi::updatePlayerTeam(std::string player_id, std::string team) {
    Connection* target_connection = getConnectionFromPlayerID(player_id);

    if (target_connection == nullptr) {
        common::print("No connection was found with the provided player_id");
        return;
    }

    target_connection->setTeam(team);
    S2CMessages::sendBroadcastTeamUpdate(m_connection_list);
}


bool ShengJi::isMessageFromExpectedPlayer(JSON message) {
    return common::stringCompare(m_expected_player->getID(), message.at("player_id"));
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

void ShengJi::startGameWithSettings(JSON message) {
    // if (message.at("team_start_recommended")) {
    //     for (auto const& curr_conn : m_connection_list) {
    //         curr_conn->setRevealElegibility(true);
    //         curr_conn->setProgressCard(DEFAULT_PROGRESS_CARD);
    //     }
    // }
    // else {
    //     std::vector<std::string> eligible_list = message.at("reveal_eligible_players");
    //     for (auto const& curr_conn : m_connection_list) {
    //         std::string player_id = curr_conn->getID();
    //         curr_conn->setRevealElegibility(common::isInVector<std::string>(eligible_list, player_id));
    //         curr_conn->setProgressCard(message["player_start_cards"][player_id]);
    //     }
    // }


    // if (message.at("deck_configuration_recommended")) {
    //     //Create recommended rules
    // }
    // else {
    //     int number_of_decks = message.at("number_of_decks");
    //     std::vector<std::string> omit_ranks = message.at("ranks_to_omit");

    //     m_deck = Deck(number_of_decks, omit_ranks);
    // }
}

void ShengJi::setPlayerRevealEligibility(std::string player_id, bool eligible) {
    Connection* target = getConnectionFromPlayerID(player_id);
    target->setRevealElegibility(eligible);
    S2CMessages::sendSetRevealElegible(target, eligible);
}

void ShengJi::dealCards() {

}