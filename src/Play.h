#pragma once

//#include "Player.h"
#include "Card.h"
#include <unordered_map>
#include <set>

class Player;

class Play
{
private:
	Player* m_source;
	set<Card*> m_cards;
	bool m_valid;
	bool m_throw;

	//Keys are
	//  m_component_count
	//  m_component_length
	unordered_map<string, int> m_structure;
	Card* m_high_card;

	unordered_map<string, int> m_throw_componenets;
	unordered_map<string, int> m_throw_low_cards;

	string m_play_name;

public:

	//Sets the player who made this play as the source
	//Must call makeStartPlay or makeFollowPlay after to fully establish the play
	Play(Player* source);

	void makeStartPlay(unordered_map<string, int> structure, set<Card*> cards);

	void makeFollowPlay(Play* starter, set<Card*> cards, bool valid);

	unordered_map<string, int> getStructure() const;

	string toString() const;

	bool isValid() const;

	Card* getHighCard() const;

	int isLessThan(const Play& other) const;

	int isGreaterThan(const Play& other) const;

	bool operator>(const Play& other) const;

	bool operator<(const Play& other) const;

private:

	void setHighCard();
	
};