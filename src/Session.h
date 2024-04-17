#pragma once
#include "Player.h"
#include "Deck.h"
#include "json.hpp"

#include <unordered_set>


enum MatchState {
	DRAWING,
	AWAITING_TRICK_START,
	AWAITING_TRICK_FOLLOW
};

class Session
{
private:
	//administrative fields
	Deck* m_deck;
	unordered_set<int> m_player_ids;
	list<Player*> m_player_list;
	list<string> m_chatlog;
	unordered_map<int, Player*> m_teams;
	unordered_map<int, int> m_match_score;


	//gametime fields
	MatchState m_state;
	int m_player_count;
	int m_game_score;
	
	Player* m_trick_starter;

	Player* m_current_player;
	Player* m_next_player;

	int m_match_state;

public:

	Session();

	void startGame();

	void addToChat(string message);

	int addPlayer(string name);

	int generateID(); 
};