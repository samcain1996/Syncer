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

    char buf[4096];

    auto x = connection.socket->send(boost::asio::buffer(filename));
    if (connection.error_code == boost::system::errc::success) {

        auto bytes_received = connection.socket->receive(boost::asio::buffer(buf));
        if (connection.error_code != boost::system::errc::success) { return file; }

        Data data;
        for (int i = 0; i < bytes_received; i++) {
            data.push_back(buf[i]);
        }
        file = std::make_pair(filename, data);
    }


    return file;

}

bool Client::Connect(const string& address, const string& port) {

    tcp::endpoint endpoint(address::from_string(address), std::stoi(port));
    connection.socket->connect(endpoint, connection.error_code);

    return connection.error_code == boost::system::errc::success;

}