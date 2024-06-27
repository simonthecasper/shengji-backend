#pragma once

#include "Card.h"


class Deck
{
private:
	std::string m_suits[4] = { "Spades", "Clubs", "Hearts", "Diamonds" };
	std::list<Card*> m_all_cards;
	std::list<Card*> m_new_deck;
	std::list<Card*> m_draw_deck;
	std::list<Card*> m_discard_deck;
	std::list<Card*> m_kitty;

	std::unordered_map<std::string, int> m_suit_rank;

	std::list<Card*>::iterator m_draw_deck_top;

public:
	Deck();

	Deck(int number_of_decks, int rank_omit_count);

	//Shuffles all cards into the draw_deck. The original deck is still maintained.
	void shuffleDeck();

	//Returns a Card from the draw_deck 
	Card* drawCard();

	//Card* drawCards(int amount);


	// Returns the pointer to the Card object with the provided suit, value, and id.
	// If no card exists with the provided identifying info, returns std::nullptr
	Card* getCard(std::string suit, std::string value, int id);


	//Prints out all cards in order of their ranks (debugging)
	std::string printRankOrder();


	//Sets the strong suit and strong value for all the cards in the deck.
	void setStrongSuitAndValue(std::string strong_suit, std::string value);


	int getDrawDeckSize() const;


	int deleteDeck();

private:
	//Creates a blank standard deck
	void createDeck(int number_of_decks, int rank_omit_count);
};

