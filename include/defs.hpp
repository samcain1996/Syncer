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

static constexpr const std::nullopt_t NoFile = std::nullopt;

class Connection {

    bool connected = false;

public:

    static inline const Data DISCONNECT_MESSAGE = { 'B', 'Y', 'E', '!' };
    static inline const int BUFFER_SIZE = 2048;

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

using Buffer = std::array<char, Connection::BUFFER_SIZE>;
