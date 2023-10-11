#include "Server.hpp"

Server::Server() {

    port = 3000;

    connection.io_service = new boost::asio::io_service();
    connection.acceptor = new tcp::acceptor(*connection.io_service, tcp::endpoint(tcp::v4(), port));
    connection.socket = new tcp::socket(*connection.io_service);

}
Server::~Server() {

    // TODO: delete connection

}

bool Server::Listen() {

    connection.acceptor->accept(*connection.socket, connection.error_code);

    return connection.error_code == boost::system::errc::success;
}