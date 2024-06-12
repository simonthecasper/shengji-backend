#pragma once

#include "common.h"
#include "Play.h"

namespace S2CMessages {

    void sendChatMessage(std::string sid, std::string player_id, std::string message);

    /*-------------------------------------------*/
    /*                Lobby Phase                */
    /*-------------------------------------------*/
    void sendSessionJoined(std::string sid, std::string room_id, std::string player_id);

    void sendSetHost(std::string sid, std::string player_id);

    void sendSetTeam(std::string sid, JSON teams);

    void sendLobbyNotReady(std::string sid);

    void sendStartDraw(std::string sid);


    /*-------------------------------------------*/
    /*                Draw Phase                 */
    /*-------------------------------------------*/
    void sendDrawCard(std::string sid, JSON card);

    void sendOtherPlayerDrawCard(std::string sid, std::string drawing_player_id);

    void sendSetStrongSuit(std::string sid, JSON card, std::string setting_player_id);

    void setLordPlayer(std::string sid, std::string player_id);

    void endMainDraw(std::string sid);

    void sendStartGame(std::string sid);


    /*-------------------------------------------*/
    /*                Game Phase                 */
    /*-------------------------------------------*/
    void sendSignalTrickStarter(std::string sid, std::string player_id);

    void sendSignalExpectedPlayer(std::string sid, std::string player_id);

    void sendBroadcastPlay(std::string sid, std::string player_id, JSON structure, Play source_play);

    void sendPlayError(std::string sid, std::string message);

    void sendSetWinningPlayer(std::string sid, std::string player_id);

    void sendSignalTrickEnd(std::string sid);

    void sendUpdateScore(std::string sid, std::list<Card*> scored_cards, int points_scored);

    void sendFinishGame(std::string sid);

}