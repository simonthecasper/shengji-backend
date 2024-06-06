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

	//Keys are
	//  m_component_count (number of unique cards)
	//  m_component_length (single, pair, triple)
	//
	//Example:
	//	- Single Ace : m_component_count = 1 | m_component_length = 1
	//  - Pair 10 :	m_component count = 1 | m_component length = 2
	//	- Consecutive Pair "5566" : m_component_count = 2 | m_component_length = 2
	unordered_map<string, int> m_structure;
	Card* m_high_card;

	string m_play_name;

public:

	Play();

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