#include "Networking.hpp"

void Connection::Disconnect() { if (connected) { SendData(DISCONNECT_MESSAGE); } connected = false; }

Connection::Connection() { connected = false; }

bool Connection::SendData(const Data& data) {
    
    if (!connected) { return false; }

    socket->send(buffer(data), 0, err_code);
    socket->receive(buffer(buf), 0, err_code);

    return (err_code == errc::success && std::memcmp(buf.data(), ACK_MESSAGE.data(), ACK_MESSAGE.size()) != 0);


}

bool Connection::SendData(const string& data) {
    
    if (!connected) { return false; }

    socket->send(buffer(data), 0, err_code);
    socket->receive(buffer(buf), 0, err_code);

    return (err_code == errc::success && std::memcmp(buf.data(), ACK_MESSAGE.data(), ACK_MESSAGE.size()) != 0);

}

size_t Connection::ReceiveData() {

   if (!connected) { return -1; }

    auto bytes = socket->receive(buffer(buf), 0, err_code);
    auto msg = ACK_MESSAGE;
    if (IsDisconnectMessage(buf)) { msg = DISCONNECT_MESSAGE; connected = false; }

    socket->send(buffer(msg), 0, err_code);

    if (err_code == errc::success) { return bytes; }
    return -1;

}

bool Connection::IsConnected() const { return connected; }

bool Connection::IsDisconnectMessage(const Buffer& message) const { 
    return std::memcmp(DISCONNECT_MESSAGE.data(), message.data(), DISCONNECT_MESSAGE.size()) == 0;
}

File ReadFile(const string& filename, const string& folder) {

    File file = NoFile;

    fstream fileStream(folder+filename, std::ios_base::in);
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

bool Client::UploadFile(const string& filename) {

    connection.SendData("upload");
    connection.SendData(filename);
    File f = ReadFile(filename, "");
    connection.SendData(f.value().second);

    return true;
}

File Client::GetFile(const string& filename) {

    File file        = NoFile;
    auto& socket     = connection.socket;
    auto& err_code = connection.err_code;

    connection.SendData("download");
    connection.SendData(filename);

    if (err_code != errc::success) { connection.Disconnect(); return file; }


        auto bytes_received = connection.ReceiveData();
        if (err_code != errc::success) { return file; }

        Data data;
        std::copy(connection.buf.begin(), connection.buf.begin() + bytes_received, back_inserter(data));
        file = make_pair(filename, data);



    return file;

}

bool Client::Connect(const string& address, const string& port) {

    tcp::endpoint endpoint(address::from_string(address), std::stoi(port));
    connection.socket->connect(endpoint, connection.err_code);

    connection.connected = connection.err_code == errc::success;
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
    auto& err_code = connection.err_code;
    auto& buf        = connection.buf;

    while(connection.IsConnected()) {

        auto bytes_received = connection.ReceiveData();
        if ( connection.IsDisconnectMessage(buf) || err_code != errc::success) { connection.Disconnect(); return; }

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
            UpdateFile(filename, data); 
        }

        connection.Disconnect();

    }

}


bool Server::UpdateFile(const string& filename, Data& data) {

    fstream file("syncedFiles/"+filename, std::ios_base::in);
    if (file.bad()) { return AddFile("syncedFiles/" + filename, data); }
    
    File f = ReadFile(filename);
    AddFile("cached/"+filename, f.value().second);
    
    file = fstream("syncedFiles/"+filename, std::ios_base::out | std::ios_base::trunc);
    file.write(data.data(), data.size());
    file.close();

    return true;

}

bool Server::AddFile(const string& filename, Data& data) {

    fstream writer(filename, std::ios_base::out);
    if (writer.bad()) { return false;}
    writer.write(data.data(), data.size());
    writer.close();
    return true;

}

bool Server::Listen() {

    connection.acceptor->accept(*connection.socket, connection.err_code);
    connection.connected = connection.err_code == errc::success;

    return connection.IsConnected();
    
}