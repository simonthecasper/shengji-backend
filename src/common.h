#pragma once


#include <algorithm>
#include <iostream>
#include <list>
#include <queue>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <functional>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <websocketpp/config/asio.hpp>
#include <websocketpp/server.hpp>
#include <websocketpp/frame.hpp>

#include "json.hpp"


// #define SERVER_PORT 		12123


using JSON = nlohmann::json;

typedef unsigned char BYTE;
typedef websocketpp::server<websocketpp::config::asio> 	ws_server;
typedef websocketpp::frame::opcode::value 				opcode;
typedef websocketpp::connection_hdl						ws_conn_hdl;
typedef ws_server::message_ptr							ws_msg_ptr;
typedef std::pair<JSON, ws_conn_hdl>					msg_hdl_pair;

namespace common {

	static std::mutex m_socket_send_mutex;

	static ws_server* m_wss;

	static int m_socketio_server;




	int sendThroughSocket(int destination, JSON message_json);

	int sendThroughSocket(int destination, std::string message_str);




	int sendThroughWebsocket(ws_conn_hdl hdl, JSON message_json);

	int sendThroughWebsocket(ws_conn_hdl hdl, std::string message);




	int sendThroughSocketSID(JSON message_json);

	int sendThroughSocketSID(std::string message_str);

	void check(int input, std::string instance);

	bool stringCompare(std::string s1, std::string s2);

	std::string base64_encode(std::string input);

	std::string base64_encode(const unsigned char* buf, int bufLen);

	std::string base64_decode(std::string const&);

	void print(std::string s);

}