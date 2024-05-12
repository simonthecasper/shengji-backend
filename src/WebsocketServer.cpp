#include "WebsocketServer.h"


WebsocketServer::WebsocketServer() {
    // Set logging settings
    m_server.set_error_channels(websocketpp::log::elevel::all);
    m_server.set_access_channels(websocketpp::log::alevel::all ^ websocketpp::log::alevel::frame_payload);

    // Initialize Asio
    m_server.init_asio();

    // Set the default message handler to the echo handler
    m_server.set_message_handler(std::bind(
        &WebsocketServer::on_message, this,
        std::placeholders::_1, std::placeholders::_2
    ));
}

void WebsocketServer::on_message(websocketpp::connection_hdl hdl, server::message_ptr msg) {
    // write a new message

    std::string response = msg->get_payload();
    std::transform(response.begin(), response.end(), response.begin(), ::toupper);

    std::cout << "payload:" << std::endl;
    std::cout << msg->get_payload() << std::endl;

    m_server.send(hdl, response, msg->get_opcode());
}

void WebsocketServer::run() {

    m_sockserver = new SocketServer();

    // Listen on SERVER_PORT
    m_server.listen(SERVER_PORT);

    // Queues a connection accept operation
    m_server.start_accept();

    // Start the Asio io_service run loop
    m_server.run();
}
