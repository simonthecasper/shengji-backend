#pragma once

#include "Deck.h"
#include "Connection.h"
#include "S2CMessages.h"

class Game {
protected:
    std::list<Connection*>      m_connection_list;
    Deck                        m_deck;

public:
    Game();

    // General message handler that routes to the correct handler
    // for the task 
    virtual void handleMessage(JSON message) {};

protected:
    virtual void createDeck(int player_count, int deck_count) {};

    // Updates a players team assignment and notifies all players in the lobby of this change
    virtual void updatePlayerTeam(std::string player_id, std::string team) {};

    Connection* getConnectionFromPlayerID(std::string player_id);
};