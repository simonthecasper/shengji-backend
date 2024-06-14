#include "Player.h"
#include "Play.h"


Player::Player() {

}

Player::Player(std::string id) {
	m_id = id;
}


Player::Player(std::string id, std::string name) {
	m_id = id;
	m_name = name;
	//m_scoring = scoring;
}

void Player::toggleScoring() {
	m_scoring = !m_scoring;
}


void Player::setSID(std::string sid) {
	m_sid = sid;
}


std::string Player::getSID() {
	return m_sid;
}


std::string Player::getName() const {
	return m_name;
}

std::string Player::getID() const {
	return m_id;
}

bool Player::isScoring() const {
	return m_scoring;
}

void Player::setTeam(std::string team) {
	m_team = team;
}

std::string Player::getTeam() const {
	return m_team;
}

void Player::addCardToHand(Card* card) {
	m_hand.insert(card);
}


bool Player::ifHasCard(std::string suit, int rank, int id) {
	std::set<Card*>::iterator itr;
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




std::set<Card*> Player::getHand() const {
	return m_hand;
}


int Player::getHandSize() const {
	return m_hand.size();
}

