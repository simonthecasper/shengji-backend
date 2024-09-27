#include "S2CMessages.h"


void S2CMessages::addScheduledMessage(Connection* connection, JSON message, time_ms send_time) {
    m_scheduled_send_mutex.lock();

    ScheduledMessage* new_message = new ScheduledMessage();
    new_message->m_message = message;
    new_message->m_send_time = send_time;
    new_message->m_recipient = connection;

    m_scheduled_send_queue.push(new_message);
    m_scheduled_send_mutex.unlock();

    updateNextSendTime();
}


ScheduledMessage* S2CMessages::removeScheduledMessage() {
    m_scheduled_send_mutex.lock();
    ScheduledMessage* ret = m_scheduled_send_queue.top();
    m_scheduled_send_queue.pop();
    m_scheduled_send_mutex.unlock();
    return ret;
}


void S2CMessages::updateNextSendTime() {
    m_scheduled_send_mutex.lock();

    if (m_scheduled_send_queue.size() > 0) {
        m_next_send_time = m_scheduled_send_queue.top()->m_send_time;
    }
    else {
        m_next_send_time = std::chrono::milliseconds::max();
    }

    m_scheduled_send_mutex.lock();
}


void S2CMessages::runScheduledMessageLoop() {
    updateNextSendTime();
    while (true) {
        if (common::getTime() > m_next_send_time) {
            ScheduledMessage* to_send = removeScheduledMessage();

            JSON message = to_send->m_message;
            Connection* recipiet = to_send->m_recipient;

            recipiet->sendMessageToUser(message);
            delete to_send;

            updateNextSendTime();
        }
    }
}


/*-------------------------------------------*/
/*                Login Stage                */
/*-------------------------------------------*/
void S2CMessages::sendBroadcastChat(std::list<Connection*> connections, std::string player_id, std::string message) {
    for (auto const& curr_conn : connections) {
        JSON s2c_message = {
            { "task", "broadcast_chat"},
            { "stage", "chat"},
            { "player_id", player_id},
            { "message", message},
            { "sid", curr_conn->getSID()}
        };
        curr_conn->sendMessageToUser(s2c_message);
    }
}


void S2CMessages::sendJoinSessionNotFound(std::string sid) {
    JSON s2c_message = {
        { "stage", "prelobby"},
        { "task", "join_session_not_found"},
        { "message", "No session was found with the provided SessionID."},
        { "sid", sid}
    };
    common::sendThroughSocketSID(s2c_message);
}


void S2CMessages::sendJoinSessionAck(Connection* user_connection, std::string session_id) {
    JSON s2c_message = {
        { "stage", "prelobby"},
        { "task", "join_session_ack"},
        { "player_id", user_connection->getID()},
        { "session_id", session_id},
        { "sid", user_connection->getSID()}
    };
    user_connection->sendMessageToUser(s2c_message);
}


/*-------------------------------------------*/
/*                Lobby Stage                */
/*-------------------------------------------*/
void S2CMessages::sendBroadcastNewPlayer(std::list<Connection*> connections, std::string player_id, std::string username) {
    JSON s2c_message = {
        { "stage", "lobby" },
        { "task", "broadcast_new_player" },
        { "player_id", player_id},
        { "username", username},
    };

    for (auto const& curr_conn : connections) {
        if (!common::stringCompare(curr_conn->getID(), player_id)) {
            curr_conn->sendMessageToUser(s2c_message);
        }
    }
}


void S2CMessages::sendSharePlayerAttributes(std::list<Connection*> connections) {
    JSON s2c_message = {
            { "stage", "lobby" },
            { "task", "share_player_attributes" },
    };

    s2c_message["player_attributes"] = {};

    for (auto const& curr_conn : connections) {
        s2c_message["player_attributes"][curr_conn->getID()] = curr_conn->getPlayerAttributes();
    }

    for (auto const& curr_conn : connections) {
        s2c_message["sid"] = curr_conn->getSID();
        curr_conn->sendMessageToUser(s2c_message);
    }
}


void S2CMessages::sendBroadcastHostPlayer(std::list<Connection*> connections, std::string host_player_id) {
    JSON s2c_message = {
        { "stage", "lobby" },
        { "task", "broadcast_host_player" },
        { "host_player_id", host_player_id},
    };

    for (auto const& curr_conn : connections)
        curr_conn->sendMessageToUser(s2c_message);
}

void S2CMessages::sendLobbyNotReady(Connection* host_player_connection, std::string message) {
    JSON s2c_message = {
        { "stage", "lobby" },
        { "task", "lobby_not_ready" },
        { "message", message },
        { "sid", host_player_connection->getSID() }
    };

    host_player_connection->sendMessageToUser(s2c_message);
}

void S2CMessages::sendBeginDrawStage(std::list<Connection*> connections) {
    JSON s2c_message = {
        { "stage", "lobby" },
        { "task", "begin_draw_stage" },
    };

    for (auto const& curr_conn : connections)
        curr_conn->sendMessageToUser(s2c_message);
}

void S2CMessages::sendStartPregame(std::list<Connection*> connections, std::string game) {
    JSON s2c_message = {
        { "stage", "lobby" },
        { "task", "start_pregame" },
        { "game", game },
    };

    for (auto const& curr_conn : connections)
        curr_conn->sendMessageToUser(s2c_message);
}


/*-------------------------------------------*/
/*              Pregame Stage                */
/*-------------------------------------------*/
void S2CMessages::sendBroadcastTeamUpdate(std::list<Connection*> connections) {
    double time = std::chrono::duration<double>(common::getTime()).count();

    JSON s2c_message = {
            { "stage", "lobby" },
            { "task", "broadcast_team_update" },
            { "time", time}
    };

    for (auto const& curr_conn : connections) {
        s2c_message["teams"][curr_conn->getID()] = curr_conn->getTeam();
    }

    for (auto const& curr_conn : connections) {
        curr_conn->sendMessageToUser(s2c_message);
    }
}




/*-------------------------------------------*/
/*                Draw Stage                 */
/*-------------------------------------------*/
void S2CMessages::sendSetRevealElegible(Connection* target_connection, bool eligible) {
    JSON s2c_message = {
        { "stage", "draw" },
        { "task", "set_reveal_eligible" },
        { "eligibile", eligible }
    };
    target_connection->sendMessageToUser(s2c_message);
}

void S2CMessages::sendDealCardTargetPlayer(Connection* target_connection, Card* card) {
    JSON s2c_message = {
        { "stage", "draw" },
        { "task", "deal_card_target_player" },
        { "card",
            {
                { "suit", card->getSuit()},
                { "value", card->getID()},
                { "id", card->getID()}
            }
        },
    };
    target_connection->sendMessageToUser(s2c_message);
}

void S2CMessages::sendDealCardOtherPlayer(std::list<Connection*> connections, std::string drawing_player_id) {
    JSON s2c_message = {
            { "stage", "draw" },
            { "task", "deal_card_other_player" },
    };

    for (auto const& curr_conn : connections) {
        if (common::stringCompare(curr_conn->getID(), drawing_player_id)) { continue; }
        curr_conn->sendMessageToUser(s2c_message);
    }
}

void S2CMessages::sendBroadcastMatchCard(std::list<Connection*> connections, Card* match_card, std::string setting_player_id) {
    JSON s2c_message = {
        { "stage", "draw" },
        { "task", "broadcast_match_card" },
        { "setting_player_id", setting_player_id },
        { "match_card",
            {
                { "suit", match_card->getSuit()},
                { "value", match_card->getID()},
                { "id", match_card->getID()}
            }
        },
    };

    for (auto const& curr_conn : connections)
        curr_conn->sendMessageToUser(s2c_message);
}

void S2CMessages::setBroadcastLordPlayer(std::list<Connection*> connections, std::string lord_player_id) {
    JSON s2c_message = {
        { "stage", "draw" },
        { "task", "broadcast_match_card" },
        { "lord_player_id", lord_player_id },
    };

    for (auto const& curr_conn : connections)
        curr_conn->sendMessageToUser(s2c_message);
}

void S2CMessages::endMainDraw(std::list<Connection*> connections) {
    JSON s2c_message = {
        { "stage", "draw" },
        { "task", "end_main_draw" },
    };

    for (auto const& curr_conn : connections)
        curr_conn->sendMessageToUser(s2c_message);
}

void S2CMessages::sendStartGame(std::list<Connection*> connections) {
    JSON s2c_message = {
        { "stage", "draw" },
        { "task", "start_game" },
    };

    for (auto const& curr_conn : connections)
        curr_conn->sendMessageToUser(s2c_message);
}


/*-------------------------------------------*/
/*                Game Stage                 */
/*-------------------------------------------*/
void S2CMessages::sendBroadcastTrickStarter(std::list<Connection*> connections, std::string player_id, int trick_number) {
    JSON s2c_message = {
        { "stage", "game" },
        { "task", "broadcast_trick_starter" },
        { "starter_player_id" , player_id },
        { "trick_number", trick_number },
    };

    for (auto const& curr_conn : connections) {
        curr_conn->sendMessageToUser(s2c_message);
    }
}

void S2CMessages::sendBroadcastExpectedPlayer(std::list<Connection*> connections, std::string player_id) {
    JSON s2c_message = {
        { "stage", "game" },
        { "task", "broadcast_expected_player" },
        { "expected_player_id", player_id },
    };

    for (auto const& curr_conn : connections) {
        curr_conn->sendMessageToUser(s2c_message);
    }
}

void S2CMessages::sendBroadcastPlay(std::list<Connection*> connections, std::string source_player_id, Play* source_play) {
    std::pair<int, int> structure = source_play->getStructure();

    JSON s2c_message = {
        { "stage", "game" },
        { "task", "broadcast_play" },
        { "source_player_id", source_player_id },
        { "structure",
            {
                { "component_count", structure.first },
                { "component_length", structure.second }
            }
        },
    };

    for (auto const& curr_conn : connections) {
        curr_conn->sendMessageToUser(s2c_message);
    }
}

void S2CMessages::sendPlayError(Connection* target_connection, std::string message) {
    JSON s2c_message = {
        { "stage", "game" },
        { "task", "play_error" },
        { "message", message },
    };
    target_connection->sendMessageToUser(s2c_message);
}

void S2CMessages::sendBroadcastWinningPlayer(std::list<Connection*> connections, std::string player_id) {
    JSON s2c_message = {
        { "stage", "game" },
        { "task", "broadcast_winning_player" },
        { "winning_player_id", player_id },
    };

    for (auto const& curr_conn : connections) {
        curr_conn->sendMessageToUser(s2c_message);
    }
}

void S2CMessages::sendBroadcastTrickEnd(std::list<Connection*> connections) {
    JSON s2c_message = {
        { "stage", "game" },
        { "task", "broadcast_trick_end" },
    };

    for (auto const& curr_conn : connections) {
        curr_conn->sendMessageToUser(s2c_message);
    }
}

void S2CMessages::sendBroadcastUpdateScore(std::list<Connection*> connections, std::list<Card*> scored_cards, int points_scored) {
    JSON s2c_message = {
            { "stage", "game" },
            { "task", "broadcast_update_score" },
            { "points_scored", points_scored },
    };

    s2c_message["scored_cards"] = {};

    for (auto const& card : scored_cards) {
        s2c_message["scored_cards"].push_back(
            {
                { "suit", card->getSuit()},
                { "value", card->getValue()},
                { "id", card->getValue()},
            });
    }

    for (auto const& curr_conn : connections) {
        s2c_message["sid"] = curr_conn->getSID();
        curr_conn->sendMessageToUser(s2c_message);
    }
}

void S2CMessages::sendFinishGame(std::list<Connection*> connections) {
    JSON s2c_message = {
        { "stage", "game" },
        { "task", "finish_game" },
    };

    for (auto const& curr_conn : connections) {
        curr_conn->sendMessageToUser(s2c_message);
    }
}
