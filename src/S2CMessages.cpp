#include "S2CMessages.h"


void S2CMessages::sendBroadcastChat(std::list<Player*> players, std::string player_id, std::string message) {
    std::list<Player*>::iterator itr;

    for (itr = players.begin(); itr != players.end(); itr++) {
        Player* curr_player = *itr;
        JSON s2c_message = {
            {"task", "chat"},
            {"player_id", player_id},
            {"message", message},
            {"sid", curr_player->getSID()}
        };
        common::sendThroughSocketSID(s2c_message);
    }
}

void S2CMessages::sendJoinSessionNotFound(std::string sid) {
    JSON s2c_message = {
        {"stage", "prelobby"},
        {"task", "join_session_not_found"},
        {"message", "No session was found with the provided SessionID."},
        {"sid", sid}
    };
    common::sendThroughSocketSID(s2c_message);
}


/*-------------------------------------------*/
/*                Lobby Stage                */
/*-------------------------------------------*/
void S2CMessages::sendJoinSessionAck(std::string sid, std::string room_id, std::string player_id) {
    JSON s2c_message = {
        {"stage", "lobby"},
        {"task", "join_session_ack"},
        {"player_id", player_id},
        {"room_id", room_id},
        {"sid", sid}
    };
    common::sendThroughSocketSID(s2c_message);
}


void S2CMessages::sendBroadcastNewPlayer(std::list<Player*> players, std::string player_id, std::string username) {
    std::list<Player*>::iterator itr;

    for (itr = players.begin(); itr != players.end(); itr++) {
        Player* curr_player = *itr;
        JSON s2c_message = {
            {"stage", "lobby" },
            { "task", "broadcast_new_player" },
            {"player_id", player_id},
            {"username", username},
            {"sid", curr_player->getSID()}
        };
        common::sendThroughSocketSID(s2c_message);
    }
}

void S2CMessages::sendBroadcastHostPlayer(std::list<Player*> players, std::string player_id) {
    std::list<Player*>::iterator itr;

    for (itr = players.begin(); itr != players.end(); itr++) {
        Player* curr_player = *itr;
        JSON s2c_message = {
            {"stage", "lobby" },
            { "task", "broadcast_host_player" },
            {"player_id", player_id},
            {"sid", curr_player->getSID()}
        };
        common::sendThroughSocketSID(s2c_message);
    }
}

void S2CMessages::sendBroadcastTeamUpdate(std::list<Player*> players, std::unordered_map<std::string, std::string> teams) {
    double time = common::getTime();

    std::list<Player*>::iterator itr;

    for (itr = players.begin(); itr != players.end(); itr++) {
        Player* curr_player = *itr;
        JSON s2c_message = {
            {"stage", "lobby" },
            { "task", "broadcast_team_update" },
            { "time", time},
            {"sid", curr_player->getSID()}
        };

        for (auto const& x : teams) {
            s2c_message["teams"][x.first] = x.second;
        }

        common::sendThroughSocketSID(s2c_message);
    }
}

void S2CMessages::sendLobbyNotReady(std::string sid, std::string message) {
    JSON s2c_message = {
        { "stage", "lobby" },
        { "task", "lobby_not_ready" },
        { "message", message },
        { "sid", sid }
    };
    common::sendThroughSocketSID(s2c_message);
}

void S2CMessages::sendBeginDrawStage(std::list<Player*> players) {
    std::list<Player*>::iterator itr;

    for (itr = players.begin(); itr != players.end(); itr++) {
        Player* curr_player = *itr;
        JSON s2c_message = {
            {"stage", "lobby" },
            { "task", "begin_draw_stage" },
            {"sid", curr_player->getSID()}
        };
        common::sendThroughSocketSID(s2c_message);
    }
}


/*-------------------------------------------*/
/*                Draw Stage                 */
/*-------------------------------------------*/
void S2CMessages::sendDealCardTargetPlayer(std::string sid, Card* card) {
    JSON s2c_message = {
        {"stage", "draw" },
        { "task", "deal_card_target_player" },
        { "card",
            {
                {"suit", card->getSuit()},
                {"value", card->getID()},
                {"id", card->getID()}
            }
        },
        {"sid", sid}
    };
    common::sendThroughSocketSID(s2c_message);
}

void S2CMessages::sendDealCardOtherPlayer(std::list<Player*> players, std::string drawing_player_id) {
    std::list<Player*>::iterator itr;

    for (itr = players.begin(); itr != players.end(); itr++) {
        Player* curr_player = *itr;

        if (curr_player->getID() == drawing_player_id) { continue; }

        JSON s2c_message = {
            {"stage", "draw" },
            { "task", "deal_card_other_player" },
            {"sid", curr_player->getSID()}
        };
        common::sendThroughSocketSID(s2c_message);
    }
}

void S2CMessages::sendBroadcastMatchCard(std::list<Player*> players, Card* match_card, std::string setting_player_id) {
    std::list<Player*>::iterator itr;

    for (itr = players.begin(); itr != players.end(); itr++) {
        Player* curr_player = *itr;

        JSON s2c_message = {
            {"stage", "draw" },
            { "task", "broadcast_match_card" },
            { "setting_player_id", setting_player_id },
            { "match_card",
                {
                    {"suit", match_card->getSuit()},
                    {"value", match_card->getID()},
                    {"id", match_card->getID()}
                }
            },
            {"sid", curr_player->getSID()}
        };
        common::sendThroughSocketSID(s2c_message);
    }
}

void S2CMessages::setBroadcastLordPlayer(std::list<Player*> players, std::string lord_player_id) {
    std::list<Player*>::iterator itr;

    for (itr = players.begin(); itr != players.end(); itr++) {
        Player* curr_player = *itr;

        JSON s2c_message = {
            {"stage", "draw" },
            { "task", "broadcast_match_card" },
            { "lord_player_id", lord_player_id },
            {"sid", curr_player->getSID()}
        };
        common::sendThroughSocketSID(s2c_message);
    }
}

void S2CMessages::endMainDraw(std::list<Player*> players) {
    std::list<Player*>::iterator itr;

    for (itr = players.begin(); itr != players.end(); itr++) {
        Player* curr_player = *itr;

        JSON s2c_message = {
            {"stage", "draw" },
            { "task", "end_main_draw" },
            {"sid", curr_player->getSID()}
        };
        common::sendThroughSocketSID(s2c_message);
    }
}

void S2CMessages::sendStartGame(std::list<Player*> players) {
    std::list<Player*>::iterator itr;

    for (itr = players.begin(); itr != players.end(); itr++) {
        Player* curr_player = *itr;

        JSON s2c_message = {
            {"stage", "draw" },
            { "task", "start_game" },
            {"sid", curr_player->getSID()}
        };
        common::sendThroughSocketSID(s2c_message);
    }
}


/*-------------------------------------------*/
/*                Game Stage                 */
/*-------------------------------------------*/
void S2CMessages::sendBroadcastTrickStarter(std::list<Player*> players, std::string player_id, int trick_number) {
    std::list<Player*>::iterator itr;

    for (itr = players.begin(); itr != players.end(); itr++) {
        Player* curr_player = *itr;

        JSON s2c_message = {
            {"stage", "game" },
            { "task", "broadcast_trick_starter" },
            { "starter_player_id" , player_id },
            { "trick_number", trick_number },
            { "sid", curr_player->getSID() }
        };
        common::sendThroughSocketSID(s2c_message);
    }
}

void S2CMessages::sendBroadcastExpecedPlayer(std::list<Player*> players, std::string player_id) {
    std::list<Player*>::iterator itr;

    for (itr = players.begin(); itr != players.end(); itr++) {
        Player* curr_player = *itr;

        JSON s2c_message = {
            {"stage", "game" },
            { "task", "broadcast_expected_player" },
            { "expected_player_id", player_id },
            { "sid", curr_player->getSID() }
        };
        common::sendThroughSocketSID(s2c_message);
    }
}

void S2CMessages::sendBroadcastPlay(std::list<Player*> players, std::string source_player_id, Play* source_play) {
    std::list<Player*>::iterator itr;
    std::pair<int, int> structure = source_play->getStructure();

    for (itr = players.begin(); itr != players.end(); itr++) {
        Player* curr_player = *itr;

        JSON s2c_message = {
            {"stage", "game" },
            { "task", "broadcast_play" },
            { "source_player_id", source_player_id },
            { "structure",
                {
                    { "component_count", structure.first },
                    { "component_length", structure.second }
                }
            },
            { "sid", curr_player->getSID() }
        };
        common::sendThroughSocketSID(s2c_message);
    }
}

void S2CMessages::sendPlayError(std::string sid, std::string message) {
    JSON s2c_message = {
        {"stage", "game" },
        { "task", "play_error" },
        { "message", message },
        { "sid", sid }
    };
    common::sendThroughSocketSID(s2c_message);
}

void S2CMessages::sendBroadcastWinningPlayer(std::list<Player*> players, std::string player_id) {
    std::list<Player*>::iterator itr;

    for (itr = players.begin(); itr != players.end(); itr++) {
        Player* curr_player = *itr;

        JSON s2c_message = {
            {"stage", "game" },
            { "task", "broadcast_winning_player" },
            { "winning_player_id", player_id },
            { "sid", curr_player->getSID() }
        };
        common::sendThroughSocketSID(s2c_message);
    }
}

void S2CMessages::sendBroadcastTrickEnd(std::list<Player*> players) {
    std::list<Player*>::iterator itr;

    for (itr = players.begin(); itr != players.end(); itr++) {
        Player* curr_player = *itr;

        JSON s2c_message = {
            {"stage", "game" },
            { "task", "broadcast_trick_end" },
            { "sid", curr_player->getSID() }
        };
        common::sendThroughSocketSID(s2c_message);
    }
}

void S2CMessages::sendBroadcastUpdateScore(std::list<Player*> players, std::list<Card*> scored_cards, int points_scored) {

    JSON s2c_message = {
            {"stage", "game" },
            { "task", "broadcast_update_score" },
            { "points_scored", points_scored },
    };

    s2c_message["scored_cards"] = {};

    for (auto const& card : scored_cards) {
        s2c_message["scored_cards"].push_back(
            {
                {"suit", card->getSuit()},
                {"value", card->getValue()},
                {"id", card->getValue()},
            });
    }

    std::list<Player*>::iterator itr;

    for (itr = players.begin(); itr != players.end(); itr++) {
        Player* curr_player = *itr;
        s2c_message["sid"] = curr_player->getSID();
        common::sendThroughSocketSID(s2c_message);
    }
}

void S2CMessages::sendFinishGame(std::list<Player*> players) {
    std::list<Player*>::iterator itr;

    for (itr = players.begin(); itr != players.end(); itr++) {
        Player* curr_player = *itr;

        JSON s2c_message = {
            {"stage", "game" },
            { "task", "finish_game" },
            { "sid", curr_player->getSID() }
        };
        common::sendThroughSocketSID(s2c_message);
    }
}
