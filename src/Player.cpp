#include "Player.h"
#include "Play.h"

Player::Player(std::string id) {
	m_id = id;
}


Player::Player(std::string id, string name) {
	m_id = id;
	m_name = name;
	//m_scoring = scoring;
}


void Player::addCardToHand(Card* card) {
	m_hand.insert(card);
}


bool Player::ifHasCard(string suit, int rank, int id) {
	set<Card*>::iterator itr;
	itr = m_hand.begin();

	while (itr != m_hand.end()) {
		Card c = **itr;
		if (c.getSuit() == suit && c.getRank() == rank && c.getID() == id) {
			return true;
		}
		itr++;
	}
	return false;
}


bool Player::ifHasCard(Card* card) {
	return m_hand.find(card) != m_hand.end();
}


//Card* Player::pickCard(Card* card) {
//	
//}


void Player::toggleScoring() {
	m_scoring = !m_scoring;
}

string Player::getName() const {
	return m_name;
}

std::string Player::getID() const {
	return m_id;
}

set<Card*> Player::getHand() const {
	return m_hand;
}


int Player::getHandSize() const {
	return m_hand.size();
}


bool Player::isScoring() const {
	return m_scoring;
}