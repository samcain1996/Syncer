#pragma once
#include <vector>
#include <iostream>
#include <string>
#include <optional>
#include <fstream>
#include <tuple>
#include <array>
#include "boost/asio.hpp"

enum class Command {
    DOWNLOAD,
    UPLOAD,
}

using Data = std::vector<unsigned char>;
using File = std::optional<std::pair<std::string, Data>>;

using std::cerr;
using std::cout;
using std::string;
using std::ifstream;
using std::ofstream;
using std::ios_base;
using std::getline;
using std::make_pair;
using std::back_inserter;
using std::tuple;
using std::make_tuple;
using namespace boost::asio::ip;
using namespace boost::asio;
using namespace boost::system;

using SocketPtr = std::unique_ptr<tcp::socket>;
using AcceptorPtr = std::unique_ptr<tcp::acceptor>;
using IO_ServicePtr = std::unique_ptr<boost::asio::io_service>;

static inline const int BUFFER_SIZE = 2048;

static constexpr const std::nullopt_t NoFile = std::nullopt;
using Buffer = std::array<char, BUFFER_SIZE>;

struct Connection {

    static inline const Data DISCONNECT_MESSAGE = { 'B', 'Y', 'E', '!' };

    Buffer sendBuf;
    Buffer receiveBuf;

    bool connected = false;
    IO_ServicePtr io_service;
    SocketPtr socket;
    AcceptorPtr acceptor;
    boost::system::error_code error_code;

    void Disconnect() { connected = false; }

    bool IsConnected() const { return connected; }

};

static File GetFile(const string& filename) {

    File file = NoFile;

    ifstream fileStream("syncedFiles/"+filename);
    if (fileStream.bad()) { return file; }
    
    Data data;
    string line;
    while (getline(fileStream, line)) {
        std::copy(line.begin(), line.end(), back_inserter(data));
    }
    file = { filename, data };

    return file;
}
