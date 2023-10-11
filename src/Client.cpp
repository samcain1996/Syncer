#include "Client.hpp"

Client::Client() {

    connection.io_service = new boost::asio::io_service();
    connection.socket = new tcp::socket(*connection.io_service);

}
Client::~Client() {

    // TODO: delete connection

}

File Client::GetFile(const string& filename) const {

    File file = std::nullopt;

    // TODO: Retrieve file

    return file;

}

bool Client::Connect(const string& address, const string& port) {

    tcp::endpoint endpoint(address::from_string(address), std::stoi(port));
    connection.socket->connect(endpoint, connection.error_code);

    return connection.error_code == boost::system::errc::success;

}