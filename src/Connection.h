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

class Connection {
private:
    ConnectionState m_state;

    Player* m_player;
    std::string m_sid;

public:
    // Basic constructor with all fields
    Connection(Player* player, std::string sid);

    // Returns a pointer to the player object associated with this Connection
    Player* getPlayer();

    // Returns the ID of the player associated with this connection
    std::string getPlayerID();

    // Sets the SID of this connnection to the provided SID if there is currently no SID
    // associated with this Connection
    //
    // Returns 0 if the SID was set to the provided value
    // Returns -1 if there is already a user and SID associated with this Connection
    int setSID(std::string sid);

    // Unassociates this Connection's SID and sets this connections state to user_disconnected
    // Returns 0 if the disconnection was successful
    // Returns -1 if the Connection was already assicoated with an SID
    int disconnectSID();

    // Returns the SID to the socketIO connection associated with this Connection
    std::string getSID();

    // Returns the username of the player associated with the connection
    std::string getUsername();

    // Send the provided message to the user if a user is connected.
    // If no user is connected, TODO: CREATE MOVE ENGINE AND SEND TO ENGINE
    int sendMessageToUser(JSON message);

    // Sets the team of the associated player to the provided team
    void setTeam(std::string team);

    // Returns the team the player on this connection is on
    std::string getTeam();
};