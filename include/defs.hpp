#pragma once
#include <vector>
#include <iostream>
#include <string>
#include <optional>
#include <fstream>
#include <tuple>
#include <array>
#include "boost/asio.hpp"

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

static inline const int BUFFER_SIZE = 2048;

static constexpr const std::nullopt_t NoFile = std::nullopt;
using Buffer = std::array<char, BUFFER_SIZE>;

class Connection {

    bool connected = false;

public:

    Buffer sendBuf;
    Buffer receiveBuf;

    static inline const Data DISCONNECT_MESSAGE = { 'B', 'Y', 'E', '!' };

    io_service* io_service;
    tcp::socket* socket;
    tcp::acceptor* acceptor;
    boost::system::error_code error_code;

    // TODO: Implement logic
    void Disconnect() { connected = false; }
    void Connect()    { connected = true; }

    bool IsConnected() const { return connected; }

    tuple<tcp::socket*, tcp::acceptor*, boost::system::error_code> AsTuple() {
        return make_tuple(socket, acceptor, error_code);
    }

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
