#include "WebsocketServer.h"


WebsocketServer::WebsocketServer() {
    // Set logging settings
    m_wss.set_error_channels(websocketpp::log::elevel::all);
    m_wss.set_access_channels(websocketpp::log::alevel::all ^ websocketpp::log::alevel::frame_payload);

    // Initialize Asio
    m_wss.init_asio();

    // Set the default message handler to the echo handler
    m_wss.set_message_handler(std::bind(
        &WebsocketServer::on_message, this,
        std::placeholders::_1, std::placeholders::_2
    ));

    common::m_wss = &m_wss;

    initThreads();
}

void WebsocketServer::initThreads() {
    pthread_t sockserver_thread;
    pthread_t wss_thread;

    pthread_create(&sockserver_thread, NULL, startSocketServer, this);
    pthread_create(&wss_thread, NULL, runWSS, this);

    pthread_join(sockserver_thread, NULL);
    pthread_join(wss_thread, NULL);

}

void WebsocketServer::on_message(ws_conn_hdl hdl, ws_msg_ptr msg) {
    // write a new message

    std::string message_str = msg->get_payload();

    std::cout << "message_str:" << message_str << std::endl;
    common::print("Adding message to queue...");

    m_sockserver->addToQueue_ws(message_str, hdl);

    // common::sendThroughWebsocket(hdl, response);
    // m_wss.send(hdl, response, msg->get_opcode());
}

void WebsocketServer::run() {
    bool unbound = true;

    while (unbound) {
        try {
            // Listen on SERVER_PORT
            m_wss.listen(SERVER_PORT);
            unbound = false;
        }
        catch (...) {
            common::print("Server socket bind failed, trying again shortly...");
            sleep(10);
        }
    }


    std::cout << "Websocket server listening on port " << SERVER_PORT << std::endl;

    // Queues a connection accept operation
    m_wss.start_accept();

    // Start the Asio io_service run loop
    m_wss.run();
}

void* WebsocketServer::startSocketServer(void* args) {
    WebsocketServer* wss = (WebsocketServer*)args;
    wss->m_sockserver = new SocketServer();

    wss->m_sockserver->runServerStandalone();

    return NULL;
}


void* WebsocketServer::runWSS(void* args) {
    WebsocketServer* wss = (WebsocketServer*)args;
    wss->run();

    return NULL;
}
