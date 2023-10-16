#include "Server.hpp"

Server::Server(const string& port) {

    connection.io_service = std::make_unique<io_service>();
    connection.acceptor   = std::make_unique<tcp::acceptor>(*connection.io_service, tcp::endpoint(tcp::v4(), std::stoi(port)));
    connection.socket     = std::make_unique<tcp::socket>(*connection.io_service);

}

Server::~Server() {}

void Server::Loop() {

    auto& socket     = connection.socket;
    auto& error_code = connection.error_code;
    auto& sendBuf    = connection.sendBuf;
    auto& receiveBuf = connection.receiveBuf;

    while(connection.IsConnected()) {

        auto bytes_received = socket->receive(buffer(receiveBuf), 0, error_code);
        socket->send(buffer(sendBuf));
        if (std::memcmp(connection.DISCONNECT_MESSAGE.data(), receiveBuf.data(), connection.DISCONNECT_MESSAGE.size()) == 0 ||
            error_code != errc::success) { connection.Disconnect(); return; }

        Buffer tmp;
        std::copy(receiveBuf.begin(), receiveBuf.begin() + bytes_received, tmp.begin());

        bytes_received = socket->receive(buffer(receiveBuf), 0, error_code);

        string filename(receiveBuf.begin(), receiveBuf.begin() + bytes_received);
        if (std::memcmp(tmp.data(), "download", bytes_received) == 0) {        
            SendFile(filename);
        }

        else { 
            socket->send(buffer(sendBuf));
            bytes_received = socket->receive(buffer(receiveBuf), 0, error_code);

            Data data; 
            std::copy(receiveBuf.begin(), receiveBuf.begin() + bytes_received, back_inserter(data)); 
            AddFile(filename, data); }

    }

}

bool Server::AddFile(const string& filename, Data& data) {

    ofstream writer("syncedFiles/"+filename);
    if (writer.bad()) { return false;}
    writer.write((char*)data.data(), data.size());
    return true;

}

bool Server::SendFile(const string& filename) {

    File file = GetFile(filename);

    if (file == NoFile) { return false; }

    connection.socket->send(buffer(file.value().second), 0, connection.error_code);
    return true;

}

bool Server::Listen() {

    connection.acceptor->accept(*connection.socket, connection.error_code);
    connection.connected = connection.error_code == errc::success;

    return connection.IsConnected();
    
}