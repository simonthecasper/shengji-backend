#pragma once

#include "Card.h"


class Deck
{
private:
	string m_suits[4] = { "Spades", "Clubs", "Hearts", "Diamonds" };
	list<Card*> m_all_cards;
	list<Card*> m_new_deck;
	list<Card*> m_draw_deck;
	list<Card*> m_discard_deck;
	list<Card*> m_kitty;

	unordered_map<string, int> m_suit_rank;

	list<Card*>::iterator m_draw_deck_top;
	
public:
	Deck(int number_of_decks, int rank_omit_count);

	//Shuffles all cards into the draw_deck. The original deck is still maintained.
	void shuffleDeck();

	//Returns a Card from the draw_deck 
	Card* drawCard();

	//Card* drawCards(int amount);
	Card* getCard(string suit, string value, int id);


	//Prints out all cards in order of their ranks (debugging)
	string printRankOrder();
	
	//Sets the strong suit and strong value for all the cards in the deck.
	void setStrongSuitAndValue(string strong_suit, string value);


	int getDrawDeckSize() const;


	int deleteDeck();

private:
	//Creates a blank standard deck
	void createDeck(int number_of_decks, int rank_omit_count,bool leaveShengJiGap);
};

