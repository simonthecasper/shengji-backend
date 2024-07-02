#pragma once

#include "Game.h"

enum GameState { draw, game };
enum DrawMode { set, compete };

class ShengJi : public Game {
private:
    std::unordered_set<std::string>	m_player_sids;
    std::list<Connection*>          m_player_order;
    Deck                        m_deck;

    std::string         m_strong_suit;
    std::string         m_strong_value;

    DrawMode    m_draw_mode;

    Connection* m_lord_player;
    Connection* m_trick_starter;
    Connection* m_expected_player;
    std::list<Connection*>::iterator m_expected_player_itr;

    Connection* m_winning_player;
    Play                m_winning_play;

    std::list<Card*>    m_all_scored_cards;
    int                 m_scored_points;

    std::list<Card*>    m_scored_cards_this_trick;

public:
    ShengJi();

    // General message handler that routes to the correct handler
    // for the task 
    void handleMessage(JSON message) override;

private:
    // Receives a play from the expected player and updates the game state accordingly
    void processExpectedPlay(JSON message);

    // Returns true if the incoming message is from the current expected player
    // Returns false if the incoming message is from a different player
    bool isMessageFromExpectedPlayer(JSON message);

    // Deals a card to the given player
    void dealCardToPlayer(Connection* target_player);

    // Notifies all other players that a player has received a new card
    void notifyOtherPlayersOfCardDeal(Connection* dealt_player);

    // Sets lord_player and informs all players in the session of this change
    void setLordPlayer(Connection* lord_player);

    // Sets trick starter player and informs all players in the session of this change
    void setTrickStarterPlayer(Connection* trick_starter);

    // Moves the expected player to the next player in the ordering
    void incrementExpectedPlayer();

    // Sets the expected player and informs all players in the session of this change
    void setExpectedPlayer(Connection* expected_player);

    // Sets the winning player and informs all players in the session of this change
    void setWinningPlayer(Connection* winning_player);

    // Sets strong suit for the game and informs all players in the session of this change
    void setStrongSuit(std::string strong_suit);

};