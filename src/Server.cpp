#include "Server.hpp"

Server::Server() {

    port = 3000;

    Data data;
    string x = "Here is some data";
    for (auto y : x) { data.push_back(y); }

    files["Test"] = data;

    connection.io_service = new boost::asio::io_service();
    connection.acceptor = new tcp::acceptor(*connection.io_service, tcp::endpoint(tcp::v4(), port));
    connection.socket = new tcp::socket(*connection.io_service);

}
Server::~Server() {

    // TODO: delete connection

}

void Server::Loop() {

    char buf[4096];
    while(connection.error_code == boost::system::errc::success) {

        auto bytes_received = connection.socket->receive(boost::asio::buffer(buf), 0, connection.error_code);
        if (connection.error_code != boost::system::errc::success) { return; }

        string filename;
        for (int i = 0; i < bytes_received; i++) { filename += buf[i]; }
        SendFile(filename);
    }

}

File Server::SendFile(const string& filename) {

    File file = std::nullopt;

    if (files.contains(filename)) {
        file = std::make_pair(filename, files.at(filename));
    }

    auto x= connection.socket->send(boost::asio::buffer(file.value().second), 0, connection.error_code);

    return file;

}

bool Server::Listen() {

    connection.acceptor->accept(*connection.socket, connection.error_code);

    return connection.error_code == boost::system::errc::success;
}