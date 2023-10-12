#include "Server.hpp"

Server::Server(const string& port) {

    InitFiles();

    connection.io_service = new io_service();
    connection.acceptor = new tcp::acceptor(*connection.io_service, tcp::endpoint(tcp::v4(), std::stoi(port)));
    connection.socket = new tcp::socket(*connection.io_service);


}
Server::~Server() {

    // TODO: delete connection

}

void Server::Loop() {

    Buffer buf;
    while(connection.IsConnected()) {

        auto bytes_received = connection.socket->receive(buffer(buf), 0, connection.error_code);
        if (std::memcmp(connection.DISCONNECT_MESSAGE.data(), buf.data(), connection.DISCONNECT_MESSAGE.size()) == 0) { connection.Disconnect(); return; }
        if (connection.error_code != errc::success) { connection.Disconnect(); return; }

        string filename;
        for (int i = 0; i < bytes_received; i++) { filename += buf[i]; }
        SendFile(filename);
    }

}

bool Server::AddFile(const string& filename, Data& data) {
    files[filename] = data;

    ofstream writer(filename);
    if (!writer.good()) { return false;}
    writer.write((char*)data.data(), data.size());
    return true;
}

bool Server::InitFiles() {

    ifstream fileStream("Test", ios_base::binary);
    if (!fileStream.good()) { return false; }

    Data data;
    string line;
    while (std::getline(fileStream, line)) {
        std::copy(line.begin(), line.end(), std::back_inserter(data));
    }
    files["Test"] = data;

    return true;
}

File Server::SendFile(const string& filename) {

    File file = std::nullopt;

    if (files.contains(filename)) {
        file = std::make_pair(filename, files.at(filename));
    }

    connection.socket->send(buffer(file.value().second), 0, connection.error_code);

    return file;

}

bool Server::Listen() {

    connection.acceptor->accept(*connection.socket, connection.error_code);
    bool connected = connection.error_code == errc::success;
    if (connected) { connection.Connect(); }
    return connection.IsConnected();
}