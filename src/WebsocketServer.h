#pragma once

#define ASIO_STANDALONE


#include "SocketServer.h"



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
    ws_server       m_wss;
    SocketServer* m_sockserver;

public:
    WebsocketServer();

    void initThreads();

    void on_message(websocketpp::connection_hdl hdl, ws_server::message_ptr msg);

    void run();

    static void* startSocketServer(void* args);

    static void* runWSS(void* args);
};
