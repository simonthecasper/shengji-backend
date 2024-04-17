#include "Session.h"


Session::Session() {

}

int Session::addPlayer(string name) {
	int id = generateID();

	Player* p = new Player(id);
	m_player_ids.insert(id);
	m_player_list.push_back(p);
	m_player_count = m_player_list.size();

	return 0;
}


int Session::generateID() {
	int id = rand() % 10000;
	while (m_player_ids.count(id) != 0)
		id = rand() % 10000;

	return id;
}