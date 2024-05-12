#pragma once

#define ASIO_STANDALONE

#include <functional>

#include <websocketpp/config/asio.hpp>
#include <websocketpp/server.hpp>

#include "SocketServer.h"


#define SERVER_PORT     12345


typedef websocketpp::server<websocketpp::config::asio> server;


class LogStream : public std::streambuf {
private:
    std::string buffer;

protected:
    int overflow(int ch) override {
        buffer.push_back((char)ch);
        if (ch == '\n') {
            // End of line, write to logging output and clear buffer.

            buffer.clear();
        }

        return ch;

        //  Return traits::eof() for failure.
    }
};



class WebsocketServer {

private:
    server m_server;
    SocketServer m_sockserver;

public:
    WebsocketServer();

    void on_message(websocketpp::connection_hdl hdl, server::message_ptr msg);

    void run();

};
