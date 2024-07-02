#pragma once
#include <set>
#include "Play.h"

class Player
{
private:
	std::string 	m_id;
	std::string 	m_username;
	std::string 	m_team;

	std::set<Card*> 		m_hand;

public:
	Player();

	Player(std::string id);

	Player(std::string id, std::string username);


	// Returns the name of the player
	std::string getUsername() const;

	// Returns the assigned ID of the player
	std::string getID() const;

	// Given a suit, rank, and id, returns whether this player has this card in
	//	their hand
	bool ifHasCard(std::string suit, int rank, int id);

	// Returns true if the requested Card* is in the players hand
	bool ifHasCard(Card* card);

	// Removes the provided card from the players hand and returns it
	// Needs to be implemented
	Card* pickCard(Card* card);

	//Adds the provided card to the hand of the player
	void addCardToHand(Card* card);

	//Returns the hand of cards this player has
	std::set<Card*> getHand() const;

	//Returns the size of the players hand
	int getHandSize() const;

	void setTeam(std::string team);

	std::string getTeam();
};