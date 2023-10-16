#include "Client.hpp"

Client::Client() {

    connection.io_service = std::make_unique<boost::asio::io_service>();
    connection.socket     = std::make_unique<tcp::socket>(*connection.io_service);

}
Client::~Client() {


}

File Client::GetFile(bool download, const string& filename) {

    File file        = NoFile;
    auto& socket     = connection.socket;
    auto& error_code = connection.error_code;
    string command   = download ? "download" : "upload"; 

    socket->send(buffer(command));
    socket->receive(buffer(connection.receiveBuf), 0, error_code);
    socket->send(buffer(filename));

    if (download && error_code == errc::success) {

        auto bytes_received = socket->receive(buffer(connection.receiveBuf), 0, error_code);
        if (error_code != errc::success) { return file; }

        Data data;
        std::copy(connection.receiveBuf.begin(), connection.receiveBuf.begin() + bytes_received, back_inserter(data));
        file = std::make_pair(filename, data);
    }
    else {
        socket->receive(buffer(connection.receiveBuf), 0, error_code);
        connection.sendBuf = {'C', 'O', 'M', 'E', ' ', 'O', 'N', '!' };
        socket->send(buffer(connection.sendBuf), 0, error_code);
    }

    socket->send(buffer(connection.DISCONNECT_MESSAGE), 0, error_code);
    connection.Disconnect();

    return file;

}

bool Client::Connect(const string& address, const string& port) {

    tcp::endpoint endpoint(address::from_string(address), std::stoi(port));
    connection.socket->connect(endpoint, connection.error_code);

    connection.connected = connection.error_code == errc::success;
    return connection.IsConnected();

}