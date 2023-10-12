#include "Client.hpp"

Client::Client() {

    connection.io_service = new io_service();
    connection.socket = new tcp::socket(*connection.io_service);

}
Client::~Client() {

    // TODO: delete connection
    delete connection.socket; 
        delete connection.io_service;

}

File Client::GetFile(bool dl, const string& filename) {

    File file = NoFile;
    auto [socket, acceptor, error_code] = connection.AsTuple();

    string dOrU = dl ? "download" : "upload";
    socket->send(buffer(dOrU));
    auto y = socket->receive(buffer(connection.receiveBuf));
    socket->send(buffer(filename));

    if (dl && error_code == errc::success) {

        auto bytes_received = socket->receive(buffer(connection.receiveBuf));
        if (error_code != errc::success) { return file; }

        Data data;
        std::copy(connection.receiveBuf.begin(), connection.receiveBuf.begin() + bytes_received, back_inserter(data));
        file = std::make_pair(filename, data);
    }
    else {
        y = socket->receive(buffer(connection.receiveBuf));
        connection.sendBuf = {'C', 'O', 'M', 'E', ' ', 'O', 'N', '!' };
        socket->send(buffer(connection.sendBuf));
    }

    socket->send(buffer(connection.DISCONNECT_MESSAGE), 0, error_code);
    connection.Disconnect();

    return file;

}

bool Client::Connect(const string& address, const string& port) {

    tcp::endpoint endpoint(address::from_string(address), std::stoi(port));
    connection.socket->connect(endpoint, connection.error_code);

    bool connected = connection.error_code == errc::success;
    if (connected) { connection.Connect(); }
    return connection.IsConnected();

}