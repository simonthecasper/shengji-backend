#include "Chat.h"


Chat::Chat() {
	m_mutex_chatlog = CreateMutex(NULL, FALSE, NULL);
}


int Chat::addToChat(int player_id, std::string message) {
	DWORD dwWaitResult = WaitForSingleObject(m_mutex_chatlog, INFINITE);
	
	JSON message_json;
	message_json["player_id"] = player_id;
	message_json["message"] = message;
	
	switch (dwWaitResult) {
		// The thread got ownership of the mutex
		case WAIT_OBJECT_0:
			m_chatlog.push_back(message_json);

			// Release ownership of the mutex object
			if (!ReleaseMutex(m_mutex_chatlog)) {
				std::cout << "Error releasing mutex for m_mutex_chatlog" << std::endl;
				exit(-1);
			}
			break;

			// The thread got ownership of an abandoned mutex
		case WAIT_ABANDONED:
			return -1;
		}
	return 0;
}
