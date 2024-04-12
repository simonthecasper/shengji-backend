#pragma once
#include <unordered_map>
#include "Player.h"
#include <unordered_set>

class Match
{
private:
	//administrative fields
	unordered_set<int> m_player_ids;
	list<Player*> m_player_list;
	list<string> m_chatlog;
	unordered_map<int, Player*> m_teams;
	unordered_map<int, int> m_match_score;

	//
	int m_player_count;
	int m_game_score;
	
	Player* m_trick_starter;

	Player* m_current_player;
	Player* m_next_player;

	int m_match_state;

public:

	Match();

	void startGame();

	void addToChat(string message);

	int addPlayer(string name);

	int generateID();
};