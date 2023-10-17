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
    
    if (!connected) { return false; }

    auto size = data.size();

    socket->send(buffer(std::to_string(size)), 0, err_code);
    socket->receive(buffer(buf), 0, err_code);

    write(*socket, buffer(data, size));

    return (err_code == errc::success && HasAcknowledged());

}

size_t Connection::ReceiveData() {

   if (!connected) { return -1; }

    auto bytes = socket->receive(buffer(buf), 0, err_code);
    auto msg = ACK_MESSAGE;
    if (IsDisconnectMessage(buf)) { msg = DISCONNECT_MESSAGE; connected = false; }

    string s;
    for (int i = 0; i < bytes; i++) {
        s += buf[i];
    }
    socket->send(buffer(msg), 0, err_code);
    bytes = read(*socket, buffer(buf, std::stoi(s)));

    if (err_code == errc::success) { return bytes; }
    return -1;

}

Data Connection::ReceiveDatar() {

   if (!connected) { return Data(); }

    auto bytes = socket->receive(buffer(buf), 0, err_code);
    string s;
    for (int i = 0; i < bytes; i++) {
        s += buf[i];
    }
    auto msg = ACK_MESSAGE;
    if (IsDisconnectMessage(buf)) { msg = DISCONNECT_MESSAGE; connected = false; }
    socket->send(buffer(msg), 0, err_code);

    Data data;
    auto size = std::stoi(s);
    while (size > 0) {

        auto bytes_to_receive = size > BUFFER_SIZE ? BUFFER_SIZE : size;
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
    }
    file = { filename, data };

    return file;
}

Client::Client(const string& archive_folder, const string& save_folder) {

    connection.SAVE_FOLDER = save_folder;
    connection.ARCHIVE_FOLDER = archive_folder;

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
    auto& err_code   = connection.err_code;

    connection.SendData("download");
    connection.SendData(filename);

    if (err_code != errc::success) { return file; }


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

Server::Server(const string& port, const string& archive_folder, const string& save_folder) {

    connection.SAVE_FOLDER = save_folder;
    connection.ARCHIVE_FOLDER = archive_folder;

    connection.io_service = std::make_unique<io_service>();
    connection.acceptor   = std::make_unique<tcp::acceptor>(*connection.io_service, tcp::endpoint(tcp::v4(), std::stoi(port)));
    connection.socket     = std::make_unique<tcp::socket>(*connection.io_service);

}

Server::~Server() {}

void Server::Loop() {

    auto& socket     = connection.socket;
    auto& err_code   = connection.err_code;
    auto& buf        = connection.buf;

    while(true) {

        auto bytes_received = connection.ReceiveData();
        if ( connection.IsDisconnectMessage(buf) || err_code != errc::success) { return; }

        bool download = std::memcmp(buf.data(), "download", bytes_received) == 0;

        bytes_received = connection.ReceiveData();
        string filename(buf.begin(), buf.begin() + bytes_received);

        if (download) {     
            File f = ReadFile(filename);
            connection.SendData(f.value().second);
        }

        else {
            Data data = connection.ReceiveDatar();
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