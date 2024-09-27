#pragma once


#include "Player.h"

/*
The Connection Class represents one users connection to a session.
The Class contains a pointer to the Player object for this player and a std::string
representing the socketIO sid for this users connection.

The primary purpose of this class is to allow users to disconnect from the server and reconnect
to open spots in existing games without massively disrupting the flow of the game.

Perhaps in the future, an intelligent Move engine can be made to make moves for Connections
that don't have connected players.

*/

enum ConnectionState { user_connected, user_disconnected };

class Connection : public Player {
private:
    ConnectionState m_state;

    // Player* m_player;
    std::string m_sid;

public:
    Connection(std::string id, std::string username, std::string sid);

    // Sets the SID of this connnection to the provided SID if there is currently no SID
    // associated with this Connection
    //
    // Returns 0 if the SID was set to the provided value
    // Returns -1 if there is already a user and SID associated with this Connection
    int setSID(std::string sid);

    // Unassociates this Connection's SID and sets this connections state to user_disconnected
    // Returns 0 if the disconnection was successful
    // Returns -1 if the Connection was already disassociated with an SID
    int disconnectSID();

    // Returns the SID to the socketIO connection associated with this Connection
    std::string getSID();

    // Send the provided message to the user if a user is connected.
    // If no user is connected, TODO: CREATE MOVE ENGINE AND SEND TO ENGINE
    int sendMessageToUser(JSON message);
};