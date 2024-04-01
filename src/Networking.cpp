#include "Networking.hpp"

bool Connection::SendData(const Data& data) {
    
    if (!connected) { return false; }
    
    socket->send(buffer(to_string(data.size())), 0, err_code);
    socket->receive(buffer(buf), 0, err_code);

    write(*socket, buffer(data, data.size()));

    return (err_code == errc::success && HasAcknowledged());

}

bool Connection::HasAcknowledged() const {
    return memcmp(buf.data(), ACK_MESSAGE.data(), ACK_MESSAGE.size()) != 0;
}

Data Connection::ReceiveData() {

    // Read the amount of data to expect
    auto bytes_received = socket->receive(buffer(buf), 0, err_code);
    auto bytes_expected = stoi(string(buf.begin(), buf.begin() + bytes_received));

    // Send acknowledgement back
    socket->send(buffer(ACK_MESSAGE), 0, err_code);

    // Extract data line by line until all the data has 
    // been extracted and stored in vector
    Data data;
    while (bytes_expected > 0) {

        bytes_received = bytes_expected >= BUFFER_SIZE ? BUFFER_SIZE : bytes_expected;
        bytes_expected -= read(*socket, buffer(buf, bytes_received));

        copy(buf.begin(), buf.begin() + bytes_received, back_inserter(data));
    }

    if (err_code == errc::success) { return data; }
    return Data();

}

bool Connection::IsConnected() const { return connected; }

bool Connection::IsDisconnectMessage(const Buffer& message) const { 
    return memcmp(DISCONNECT_MESSAGE.data(), message.data(), DISCONNECT_MESSAGE.size()) == 0;
}

File ReadFile(const string& filename, const string& folder) {

    File file = NoFile;

    // Open file
    ifstream fileStream(folder+filename, ios_base::binary);
    if (fileStream.bad()) { return file; }
    
    // Read file line by line and store in vector
    Data data;
    string line;
    while (getline(fileStream, line)) {
        copy(line.begin(), line.end(), back_inserter(data));
        data.push_back('\n');
    }
    file = { filename, data };

    return file;
}

Client::Client() {

    connection.io_context = make_unique<boost::asio::io_context>();
    connection.socket     = make_unique<tcp::socket>(*connection.io_context);

}

string Client::GetContents() {

    // TODO
    connection.SendData({ LIST });

    Data response = connection.ReceiveData();
    
    return string(response.begin(), response.end());
}

bool Client::UploadFile(const string& filename) {

    connection.SendData({ UPLOAD });
    connection.SendData(Data(filename.begin(), filename.end()));
    connection.SendData(ReadFile(filename, "").value().second);

    return true;
}

File Client::GetFile(const string& filename) {

    // Send download command
    File file          = NoFile;
    connection.SendData({ DOWNLOAD });
    connection.SendData(Data(filename.begin(), filename.end()));

    if (connection.err_code != errc::success) { return file; }

    // Download file
    Data data = connection.ReceiveData();
    file = make_pair(filename, data);

    return file;

}

bool Client::Connect(const string& address, const string& port) {

    tcp::endpoint endpoint(address::from_string(address), stoi(port));
    connection.socket->connect(endpoint, connection.err_code);

    connection.connected = connection.err_code == errc::success;
    return connection.IsConnected();

}

bool Connection::portOpen(unsigned short port) {
    boost::asio::io_context i;
    tcp::acceptor a(i);
    error_code ec;

    // Check if port is in use
    a.open(tcp::v4(), ec) || a.bind( { tcp::v4(), port }, ec);
    return (ec != boost::asio::error::address_in_use);
}

Server::Server(const string& port) {

    unsigned short p = stoi(port);
    connection.io_context = make_unique<io_context>();

    // Try to listen on port, if port is not open, try next
    while (!Connection::portOpen(p)) { p++; }
    std::clog << "Server started on port: " << to_string(p) << "\n";

    connection.acceptor   = make_unique<tcp::acceptor>(*connection.io_context, tcp::endpoint(tcp::v4(), p));
    connection.socket     = make_unique<tcp::socket>(*connection.io_context);

}

void Server::Loop() {

while(true) {
        // Receive request from client
        ServerCommand command = static_cast<ServerCommand>(connection.ReceiveData()[0]);

        string filename;
        Data data;
        File f;
        switch (command) {
            case LIST:
                ListFiles();
                break;

            case UPLOAD:
                // Receive filename from client
                data = connection.ReceiveData();
                filename = {data.begin(), data.end()};

                data = connection.ReceiveData();
                UpdateFile(filename, data); 
                break;

            case DOWNLOAD:
                // Receive filename from client
                data = connection.ReceiveData();
                filename = {data.begin(), data.end()};

                // Perform request
                f = ReadFile(filename);
                connection.SendData(f.value().second);
                break;

        }

    connection.connected = false;
    connection.socket->close();
    Listen();
}

}


bool Server::UpdateFile(const string& filename, Data& data) {

    // Add file if it doesn't exist
    ifstream file(connection.SAVE_FOLDER+filename,  ios_base::binary);
    string tmp;
    getline(file, tmp);
    if (file.bad() || tmp.empty()) { return AddFile(connection.SAVE_FOLDER + filename, data); }
    
    // Archive file if it does exist then update
    File f = ReadFile(filename);
    AddFile(connection.ARCHIVE_FOLDER+filename, f.value().second);
    ofstream(connection.SAVE_FOLDER+filename, ios_base::binary).write(data.data(), data.size());

    return true;

}

bool Server::AddFile(const string& filename, Data& data) {

    ofstream writer(filename, ios_base::binary);
    if (writer.bad()) { return false;}

    writer.write(data.data(), data.size());
    return true;

}

void Server::ListFiles() {

    ostringstream fns;
    string p = current_path().string() + current_path().preferred_separator + Connection::SAVE_FOLDER;

    for_each(directory_iterator { path{p} }, 
        [&fns](const path& entry){ fns << entry.filename().string() << "\n"; });

    string filenames = fns.str();
    connection.SendData(Data(filenames.begin(), filenames.end()));

}

bool Server::Listen() {

    connection.acceptor->accept(*connection.socket, connection.err_code);
    connection.connected = connection.err_code == errc::success;

    return connection.IsConnected();
    
}
