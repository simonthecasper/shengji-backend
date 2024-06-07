#pragma once
#include <set>
#include "Play.h"

class Player
{
private:
	std::string 	m_id;
	std::string 	m_name;

	bool 			m_scoring;
	std::string 	m_team;

	set<Card*> 		m_hand;

public:
	Player();

	Player(std::string id);

	Player(std::string id, string name);


	// Changes scoring status of player
	void toggleScoring();

	// Returns the name of the player
	string getName() const;

	// Returns the assigned ID of the player
	std::string getID() const;

	// Returns whether this player is scoring or not
	bool isScoring() const;

	// Sets the player to the provided team
	void setTeam(std::string team);

	// Returns the team of the player
	std::string getTeam() const;

	// Given a suit, rank, and id, returns whether this player has this card in
	//	their hand
	bool ifHasCard(string suit, int rank, int id);

	// Returns true if the requested Card* is in the players hand
	bool ifHasCard(Card* card);

	// Removes the provided card from the players hand and returns it
	// Needs to be implemented
	Card* pickCard(Card* card);

	//Adds the provided card to the hand of the player
	void addCardToHand(Card* card);

	//Returns the hand of cards this player has
	set<Card*> getHand() const;

	//Returns the size of the players hand
	int getHandSize() const;

	//WIP: creates a play based on the received input from the client
	//Play makePlay();

};