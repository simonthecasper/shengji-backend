#pragma once

#include "common.h"
#include "Connection.h"
#include "Play.h"
#include "Player.h"

struct ScheduledMessage {
    JSON        m_message;
    time_ms     m_send_time;
    Connection* m_recipient;

    bool operator<(const ScheduledMessage& right) {
        return m_send_time < right.m_send_time;
    }
};

namespace S2CMessages {

    static time_ms                                  m_next_send_time;
    static std::priority_queue<ScheduledMessage*>   m_scheduled_send_queue;
    static std::mutex                               m_scheduled_send_mutex;

    // Adds a messages to the schedule queue with the provided send time
    void addScheduledMessage(Connection* connection, JSON message, time_ms send_time);

    // Removes the frontmost scheduled message
    ScheduledMessage* removeScheduledMessage();

    // Updates when the next time a scheduled message needs to be sent
    void updateNextSendTime();

    // Thread function to constatly poll the scheduled message queue and send the
    // scheduled messages when the time is right
    void runScheduledMessageLoop();



    /////////////////////////////////////////////////////////////
    ///////////////// Message Send Functions ////////////////////
    /////////////////////////////////////////////////////////////

    /*-------------------------------------------*/
    /*                   Chat                    */
    /*-------------------------------------------*/

    //Sends a chat message to the user with the provided sid
    void sendBroadcastChat(std::list<Connection*> connections, std::string player_id, std::string message);


    /*-------------------------------------------*/
    /*                Lobby Stage                */
    /*-------------------------------------------*/
    void sendJoinSessionNotFound(std::string sid);

    void sendJoinSessionAck(Connection* user_connection, std::string session_id);

    void sendBroadcastNewPlayer(std::list<Connection*> connections, std::string player_id, std::string username);

    void sendShareLobbyInfo(std::list<Connection*> connections, std::string host_player_id, Connection* target_connection);

    void sendBroadcastHostPlayer(std::list<Connection*> connections, std::string host_player_id);

    void sendLobbyNotReady(Connection* host_player_connection, std::string message);

    void sendBeginDrawStage(std::list<Connection*> connections);

    void sendStartPregame(std::list<Connection*> connections, std::string game);


    /*-------------------------------------------*/
    /*              Pregame Stage                */
    /*-------------------------------------------*/
    void sendBroadcastTeamUpdate(std::list<Connection*> connections);


    /*-------------------------------------------*/
    /*                Draw Stage                 */
    /*-------------------------------------------*/
    void sendDealCardTargetPlayer(Connection* target_connection, Card* card);

    void sendDealCardOtherPlayer(std::list<Connection*> connections, std::string drawing_player_id);

    void sendBroadcastMatchCard(std::list<Connection*> connections, Card* card, std::string setting_player_id);

    void setBroadcastLordPlayer(std::list<Connection*> connections, std::string player_id);

    void endMainDraw(std::list<Connection*> connections);

    void sendStartGame(std::list<Connection*> connections);


    /*-------------------------------------------*/
    /*                Game Stage                 */
    /*-------------------------------------------*/
    void sendBroadcastTrickStarter(std::list<Connection*> connections, std::string player_id, int trick_number);

    void sendBroadcastExpectedPlayer(std::list<Connection*> connections, std::string player_id);

    void sendBroadcastPlay(std::list<Connection*> connections, std::string player_id, Play* source_play);

    void sendPlayError(Connection* target_connection, std::string message);

    void sendBroadcastWinningPlayer(std::list<Connection*> connections, std::string player_id);

    void sendBroadcastTrickEnd(std::list<Connection*> connections);

    void sendBroadcastUpdateScore(std::list<Connection*> connections, std::list<Card*> scored_cards, int points_scored);

    void sendFinishGame(std::list<Connection*> connections);

}