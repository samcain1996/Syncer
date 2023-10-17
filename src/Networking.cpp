#include "Networking.hpp"

bool Connection::SendData(const Data& data) {
    
    if (!connected) { return false; }

    auto size = data.size();
    
    socket->send(buffer(std::to_string(size)), 0, err_code);
    socket->receive(buffer(buf), 0, err_code);

    write(*socket, buffer(data, size));

    return (err_code == errc::success && HasAcknowledged());


}

bool Connection::HasAcknowledged() const {
    return std::memcmp(buf.data(), ACK_MESSAGE.data(), ACK_MESSAGE.size()) != 0;
}

bool Connection::SendData(const string& data) {
    
    return SendData(Data(data.begin(), data.end()));

}

Data Connection::ReceiveData() {

   if (!connected) { return Data(); }

    auto bytes = socket->receive(buffer(buf), 0, err_code);
    auto size = std::stoi(string(buf.begin(), buf.begin() + bytes));

    socket->send(buffer(ACK_MESSAGE), 0, err_code);

    Data data;
    while (size > 0) {

        auto bytes_to_receive = size >= BUFFER_SIZE ? BUFFER_SIZE : size;
        size -= read(*socket, buffer(buf, size));

        std::copy(buf.begin(), buf.begin() + bytes_to_receive, back_inserter(data));
    }

    if (err_code == errc::success) { return data; }
    return Data();

}

bool Connection::IsConnected() const { return connected; }

bool Connection::IsDisconnectMessage(const Buffer& message) const { 
    return std::memcmp(DISCONNECT_MESSAGE.data(), message.data(), DISCONNECT_MESSAGE.size()) == 0;
}

File ReadFile(const string& filename, const string& folder) {

    File file = NoFile;

    std::ifstream fileStream(folder+filename, std::ios_base::binary);
    if (fileStream.bad()) { return file; }
    
    Data data;
    string line;
    while (getline(fileStream, line)) {
        std::copy(line.begin(), line.end(), back_inserter(data));
        data.push_back('\n');
    }
    file = { filename, data };

    return file;
}

Client::Client() {

    connection.io_context = make_unique<boost::asio::io_context>();
    connection.socket     = make_unique<tcp::socket>(*connection.io_context);

}

Client::~Client() {}

bool Client::UploadFile(const string& filename) {

    connection.SendData("upload");
    connection.SendData(filename);
    connection.SendData(ReadFile(filename).value().second);

    return true;
}

File Client::GetFile(const string& filename) {

    File file        = NoFile;
    auto& socket     = connection.socket;
    auto& err_code   = connection.err_code;

    connection.SendData("download");
    connection.SendData(filename);

    if (err_code != errc::success) { return file; }

        Data data = connection.ReceiveData();
        file = make_pair(filename, data);

    return file;

}

bool Client::Connect(const string& address, const string& port) {

    tcp::endpoint endpoint(address::from_string(address), std::stoi(port));
    connection.socket->connect(endpoint, connection.err_code);

    connection.connected = connection.err_code == errc::success;
    return connection.IsConnected();

}

bool portOpen(unsigned short port) {
    io_context i;
    tcp::acceptor a(i);
    error_code ec;
    a.open(tcp::v4(), ec) || a.bind( { tcp::v4(), port }, ec);
    return (ec != boost::asio::error::address_in_use);
}

Server::Server(const string& port) {

    unsigned short p = std::stoi(port);

    connection.io_context = make_unique<io_context>();
    while (!portOpen(p)) { p++; }
    connection.acceptor   = make_unique<tcp::acceptor>(*connection.io_context, tcp::endpoint(tcp::v4(), p));
    connection.socket     = make_unique<tcp::socket>(*connection.io_context);

}

Server::~Server() {}

void Server::Loop() {

    auto& socket     = connection.socket;
    auto& err_code   = connection.err_code;
    auto& buf        = connection.buf;

    while(true) {

        Data data = connection.ReceiveData();
        if ( connection.IsDisconnectMessage(buf) || err_code != errc::success) { return; }

        bool download = std::memcmp(data.data(), "download", data.size()) == 0;

        data = connection.ReceiveData();
        string filename(data.begin(), data.end());

        if (download) {     
            File f = ReadFile(filename);
            connection.SendData(f.value().second);
        }

        else {
            Data data = connection.ReceiveData();
            UpdateFile(filename, data); 
        }

        connection.connected = false;
        connection.socket->close();
        Listen();

    }

}


bool Server::UpdateFile(const string& filename, Data& data) {

    std::ifstream file(connection.SAVE_FOLDER+filename,  std::ios_base::binary);
    if (file.bad()) { return AddFile(connection.SAVE_FOLDER + filename, data); }
    
    File f = ReadFile(filename);
    AddFile(connection.ARCHIVE_FOLDER+filename, f.value().second);
    
    std::ofstream file2(connection.SAVE_FOLDER+filename, std::ios_base::binary);
    file2.write(data.data(), data.size());
    file2.close();

    return true;

}

bool Server::AddFile(const string& filename, Data& data) {

    std::ofstream writer(filename, std::ios_base::binary);
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