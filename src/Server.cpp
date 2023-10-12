#include "Server.hpp"

Server::Server(const string& port) {

    connection.io_service = new io_service();
    connection.acceptor = new tcp::acceptor(*connection.io_service, tcp::endpoint(tcp::v4(), std::stoi(port)));
    connection.socket = new tcp::socket(*connection.io_service);


}
Server::~Server() {

    // TODO: delete connection
    delete connection.acceptor;
    delete connection.socket; 
    delete connection.io_service;

}

void Server::Loop() {

    auto [socket, acceptor, error_code] = connection.AsTuple();

    Buffer buf;
    while(connection.IsConnected()) {

        auto bytes_received = socket->receive(buffer(buf), 0, error_code);
        if (std::memcmp(connection.DISCONNECT_MESSAGE.data(), buf.data(), connection.DISCONNECT_MESSAGE.size()) == 0 ||
            error_code != errc::success) { connection.Disconnect(); return; }

        string filename(buf.begin(), buf.begin() + bytes_received);
        SendFile(filename);

        Data newData;
        string str = "Howdy partner! This is a new file that is saved";
        std::copy(str.begin(), str.end(), back_inserter(newData));
        AddFile(filename + "New", newData);
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
    bool connected = connection.error_code == errc::success;
    if (connected) { connection.Connect(); }
    return connection.IsConnected();
}