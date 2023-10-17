#include "Networking.hpp"


void Connection::Disconnect() { connected = false; }

bool Connection::SendData(Data& data) {
    
    if (!connected) { return false; }

    socket->send(buffer(data), 0, error_code);
    socket->receive(buffer(buf), 0, error_code);

    return (error_code == errc::success && std::memcmp(buf.data(), ACK_MESSAGE.data(), ACK_MESSAGE.size()) != 0);


}

bool Connection::SendData(const string& data) {
    
    if (!connected) { return false; }

    socket->send(buffer(data), 0, error_code);
    socket->receive(buffer(buf), 0, error_code);

    return (error_code == errc::success && std::memcmp(buf.data(), ACK_MESSAGE.data(), ACK_MESSAGE.size()) != 0);


}

size_t Connection::ReceiveData() {

   if (!connected) { return -1; }

    auto bytes = socket->receive(buffer(buf), 0, error_code);
    socket->send(buffer(ACK_MESSAGE), 0, error_code);

    if (error_code == errc::success) { return bytes; }
    return -1;

}

bool Connection::IsConnected() const { return connected; }

File ReadFile(const string& filename, const string& folder) {

    File file = NoFile;

    ifstream fileStream(folder+filename);
    if (fileStream.bad()) { return file; }
    
    Data data;
    string line;
    while (getline(fileStream, line)) {
        std::copy(line.begin(), line.end(), back_inserter(data));
    }
    file = { filename, data };

    return file;
}

Client::Client() {

    connection.io_service = std::make_unique<boost::asio::io_service>();
    connection.socket     = std::make_unique<tcp::socket>(*connection.io_service);

}
Client::~Client() {


}

bool Client::UploadFile(string filename) {

    connection.SendData("upload");
    connection.SendData(filename);
    File f = ReadFile(filename, "");
    connection.SendData(f.value().second);

    return true;
}

File Client::GetFile(string filename) {

    File file        = NoFile;
    auto& socket     = connection.socket;
    auto& error_code = connection.error_code;

    connection.SendData("download");
    connection.SendData(filename);

    if (error_code != errc::success) { connection.Disconnect(); return file; }


        auto bytes_received = connection.ReceiveData();
        if (error_code != errc::success) { return file; }

        Data data;
        std::copy(connection.buf.begin(), connection.buf.begin() + bytes_received, back_inserter(data));
        file = make_pair(filename, data);



    return file;

}

bool Client::Connect(const string& address, const string& port) {

    tcp::endpoint endpoint(address::from_string(address), std::stoi(port));
    connection.socket->connect(endpoint, connection.error_code);

    connection.connected = connection.error_code == errc::success;
    return connection.IsConnected();

}

Server::Server(const string& port) {

    connection.io_service = std::make_unique<io_service>();
    connection.acceptor   = std::make_unique<tcp::acceptor>(*connection.io_service, tcp::endpoint(tcp::v4(), std::stoi(port)));
    connection.socket     = std::make_unique<tcp::socket>(*connection.io_service);

}

Server::~Server() {}

void Server::Loop() {

    auto& socket     = connection.socket;
    auto& error_code = connection.error_code;
    auto& buf        = connection.buf;

    while(connection.IsConnected()) {

        auto bytes_received = connection.ReceiveData();
        if (std::memcmp(connection.DISCONNECT_MESSAGE.data(), buf.data(), connection.DISCONNECT_MESSAGE.size()) == 0 ||
            error_code != errc::success) { connection.Disconnect(); return; }

        bool download = std::memcmp(buf.data(), "download", bytes_received) == 0;

        bytes_received = connection.ReceiveData();
        string filename(buf.begin(), buf.begin() + bytes_received);

        if (download) {     
            File f = ReadFile(filename);
            connection.SendData(f.value().second);
        }

        else { 
            bytes_received = connection.ReceiveData();

            Data data; 
            std::copy(buf.begin(), buf.begin() + bytes_received, back_inserter(data)); 
            AddFile(filename, data); 
        }

        connection.Disconnect();

    }

}

bool Server::AddFile(const string& filename, Data& data) {

    ofstream writer("syncedFiles/"+filename);
    if (writer.bad()) { return false;}
    writer.write((char*)data.data(), data.size());
    return true;

}

bool Server::Listen() {

    connection.acceptor->accept(*connection.socket, connection.error_code);
    connection.connected = connection.error_code == errc::success;

    return connection.IsConnected();
    
}