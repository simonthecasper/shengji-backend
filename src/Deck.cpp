#include "Deck.h"
#include <time.h>
#include <iostream>

Deck::Deck(int number_of_decks, int rank_omit_count) {
	m_all_cards = {};
	m_new_deck = {};
	m_draw_deck = {};
	m_discard_deck = {};
	m_kitty = {};
	
	createDeck(number_of_decks, rank_omit_count, true);
}


void Deck::createDeck(int number_of_decks, int rank_omit_count, bool leaveShengJiGap) {
	int id = 0;
	for (int deck = 0; deck < number_of_decks; deck++) {
		for (string suit : m_suits) {
			//Create 2-A of all suits
			for (int rank = 2 + rank_omit_count; rank < 15; rank++) {
				Card* newCard = new Card(suit, rank, id);
				m_new_deck.push_back(newCard);
				m_all_cards.push_back(newCard);
				id++;
			}
		}
		Card* smallJoker = new Card("Joker", 16, id++);
		Card* bigJoker = new Card("Joker", 17, id++);

		m_new_deck.push_back(smallJoker);
		m_all_cards.push_back(smallJoker);

		m_new_deck.push_back(bigJoker);
		m_all_cards.push_back(bigJoker);
	}
}


void Deck::shuffleDeck() {
	list<Card*>::iterator m_new_deck_top = m_new_deck.begin();
	list<Card*>::iterator m_new_deck_search = m_new_deck.begin();

	srand(time(NULL));

	while (m_new_deck.size() > 0) {
		m_new_deck_search = m_new_deck.begin();
		int card_pull = rand() % m_new_deck.size();

		advance(m_new_deck_search, card_pull);
		m_draw_deck.push_back(*m_new_deck_search);
		//std::cout << (**m_new_deck_search).getValue() << std::endl;
		m_new_deck.erase(m_new_deck_search);
	}

	m_draw_deck_top = m_draw_deck.begin();
}


Card* Deck::drawCard() {
	m_draw_deck_top = m_draw_deck.begin();
	Card* drawn_card = (*m_draw_deck_top);
	m_discard_deck.push_back(drawn_card);
	m_draw_deck.erase(m_draw_deck_top);
	return drawn_card;
}


Card* Deck::getCard(string suit, string value, int id){
	list<Card*>::iterator  itr = m_all_cards.begin();
	int i = 0;
	while (i != id) {
		itr++;
		i++;
	}
	Card* card = (*itr);
	if (card->getSuit() == suit && card->getValue() == value && card->getID() == id) {
		return card;
	}
	else {
		return nullptr;
	}
}



string Deck::printRankOrder() {
	list<Card*>::iterator draw_search = m_all_cards.begin();
	int pos = 0;

	string ret = "";

	for (int i = 2; i < 18; i++) {
		ret += to_string(i) + ": ";
		draw_search = m_all_cards.begin();
		pos = 0;
		while (pos < m_all_cards.size()) {
			if ((**draw_search).getRank() == i) {			//*draw_search is a Card*.
				ret += (**draw_search).getValue() + " ";	//**draw_search is a Card.
			}
			draw_search++;
			pos++;
		}
		ret += "\n";
	}

	std::cout << ret << std::endl;
	return ret;
}


int Deck::getDrawDeckSize() const {
	return m_draw_deck.size();
}


void Deck::setStrongSuitAndValue(string strong_suit, string value) {
	unordered_map<string, int> value_to_rank;
	value_to_rank["2"] = 2;
	value_to_rank["3"] = 3;
	value_to_rank["4"] = 4;
	value_to_rank["5"] = 5;
	value_to_rank["6"] = 6;
	value_to_rank["7"] = 7;
	value_to_rank["8"] = 8;
	value_to_rank["9"] = 9;
	value_to_rank["10"] = 10;
	value_to_rank["Jack"] = 11;
	value_to_rank["Queen"] = 12;
	value_to_rank["King"] = 13;
	value_to_rank["Ace"] = 14;
	
	list<Card*>::iterator all_card_iter = m_all_cards.begin();
	int pos = 0;

	while (pos < m_all_cards.size()) {
		(**all_card_iter).setStrongSuitAndRank(strong_suit, value_to_rank[value]);
		all_card_iter++;
		pos++;
	}
}


int Deck::deleteDeck() {
	return 0;
}