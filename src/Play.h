#pragma once


#include "Card.h"
#include <unordered_map>
#include <set>

class Player;

class Play
{
private:
	Player* m_source;
	std::set<Card*> m_cards;
	bool m_valid;


	//Example:
	//	- Single Ace : m_component_count = 1 | m_component_length = 1
	//  - Pair 10 :	m_component count = 1 | m_component length = 2
	//	- Consecutive Pair "5566" : m_component_count = 2 | m_component_length = 2
	int m_component_count;
	int m_component_length;

	Card* m_high_card;

	std::string m_play_name;

public:

	Play();

	//Sets the player who made this play as the source
	//Must call makeStartPlay or makeFollowPlay after to fully establish the play
	Play(Player* source);

	// void makeStartPlay(std::unordered_map<std::string, int> structure, std::set<Card*> cards);

	// void makeFollowPlay(Play* starter, std::set<Card*> cards, bool valid);

	std::pair<int, int> getStructure() const;

	std::string toString() const;

	bool isValid() const;

	Card* getHighCard() const;

	int isLessThan(const Play& other) const;

	int isGreaterThan(const Play& other) const;

	bool operator>(const Play& other) const;

	bool operator<(const Play& other) const;

private:

	void setHighCard();

};