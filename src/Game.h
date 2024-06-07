#pragma once


#include "Deck.h"
#include "Player.h"

enum GameStage { draw, game };

class Game {
private:
    enum GameStage m_game_stage;

    std::list<Player*>  m_player_order;
    Deck                m_deck;

    std::string         m_strong_suit;
    std::string         m_strong_value;

    Player* m_lord_player;
    Player* m_trick_starter;
    Player* m_expected_player;
    list<Player*>::iterator m_expected_player_itr;

    Player* m_winning_player;
    Play                m_winning_play;

    std::list<Card*>    m_all_scored_cards;
    int                 m_scored_points;

    std::list<Card*>    m_scored_cards_this_trick;

public:
    Game();

    // General message handler that routes to the correct handler
    // for the task 
    void handleMessage(JSON message);


private:
    // Receives a play from the expected player and updates the game state accordingly
    void processExpectedPlay(JSON message);

    // Returns true if the incoming message is from the current expected player
    // Returns false if the incoming message is from a different player
    bool verifyIncomingMessageIsFromExpectedPlayer(JSON message);

    // Deals a card to the given player
    void dealCardToPlayer(Player* target_player);

    // Notifies all other players that a player has received a new card
    void notifyOtherPlayersOfCardDeal(Player* dealt_player);

    // Sets lord_player and informs all players in the session of this change
    void setLordPlayer(Player* lord_player);

    // Sets trick starter player and informs all players in the session of this change
    void setTrickStarterPlayer(Player* trick_starter);

    // Moves the expected player to the next player in the ordering
    void incrementExpectedPlayer();

    // Sets the expected player and informs all players in the session of this change
    void setExpectedPlayer(Player* expected_player);

    // Sets the winning player and informs all players in the session of this change
    void setWinningPlayer(Player* winning_player);

    // Sets strong suit for the game and informs all players in the session of this change
    void setStrongSuit(std::string strong_suit);

};