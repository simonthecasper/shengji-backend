#pragma once

#include "common.h"
#include "Play.h"
#include "Player.h"



namespace S2CMessages {

    //Sends a chat message to the user with the provided sid

    void sendBroadcastChat(std::list<Player*> players, std::string player_id, std::string message);

    /*-------------------------------------------*/
    /*                Lobby Stage                */
    /*-------------------------------------------*/
    void sendJoinSessionNotFound(std::string sid);

    void sendJoinSessionAck(std::string sid, std::string room_id, std::string player_id);

    void sendBroadcastNewPlayer(std::list<Player*> players, std::string player_id, std::string username);

    void sendBroadcastHostPlayer(std::list<Player*> players, std::string player_id);

    void sendBroadcastTeamUpdate(std::list<Player*> players, std::unordered_map<std::string, std::string> teams);

    void sendLobbyNotReady(std::string sid, std::string message);

    void sendBeginDrawStage(std::list<Player*> players);


    /*-------------------------------------------*/
    /*                Draw Stage                 */
    /*-------------------------------------------*/
    void sendDealCardTargetPlayer(std::string sid, Card* card);

    void sendDealCardOtherPlayer(std::list<Player*> players, std::string drawing_player_id);

    void sendBroadcastMatchCard(std::list<Player*> players, Card* card, std::string setting_player_id);

    void setBroadcastLordPlayer(std::list<Player*> players, std::string player_id);

    void endMainDraw(std::list<Player*> players);

    void sendStartGame(std::list<Player*> players);


    /*-------------------------------------------*/
    /*                Game Stage                 */
    /*-------------------------------------------*/
    void sendBroadcastTrickStarter(std::list<Player*> players, std::string player_id, int trick_number);

    void sendBroadcastExpecedPlayer(std::list<Player*> players, std::string player_id);

    void sendBroadcastPlay(std::list<Player*> players, std::string player_id, Play* source_play);

    void sendPlayError(std::string sid, std::string message);

    void sendBroadcastWinningPlayer(std::list<Player*> players, std::string player_id);

    void sendBroadcastTrickEnd(std::list<Player*> players);

    void sendBroadcastUpdateScore(std::list<Player*> players, std::list<Card*> scored_cards, int points_scored);

    void sendFinishGame(std::list<Player*> players);

}