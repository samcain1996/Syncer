#include "Client.hpp"

Client::Client() {

    connection.io_service = new io_service();
    connection.socket = new tcp::socket(*connection.io_service);

}
Client::~Client() {

    // TODO: delete connection

}

File Client::GetFile(const string& filename) {

    File file = std::nullopt;
    auto [socket, acceptor, error_code] = connection.AsTuple();

    Buffer buf;
    socket->send(buffer(filename));
    if (error_code == errc::success) {

        auto bytes_received = socket->receive(buffer(buf));
        if (error_code != errc::success) { return file; }

        Data data;
        std::copy(buf.begin(), buf.begin() + bytes_received, std::back_inserter(data));
        file = std::make_pair(filename, data);
    }

    socket->send(buffer(connection.DISCONNECT_MESSAGE));
    connection.Disconnect();

    return file;

}

bool Client::Connect(const string& address, const string& port) {

    tcp::endpoint endpoint(address::from_string(address), std::stoi(port));
    connection.socket->connect(endpoint, connection.error_code);

    bool connected = connection.error_code == boost::system::errc::success;
    if (connected) { connection.Connect(); }
    return connection.IsConnected();

}