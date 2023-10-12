#pragma once
#include <vector>
#include <iostream>
#include <string>
#include <optional>
#include "boost/asio.hpp"

using Data = std::vector<unsigned char>;
using File = std::optional<std::pair<std::string, Data>>;

using std::cerr;
using std::cout;
using std::string;
using namespace boost::asio::ip;

struct Connection {

    boost::system::error_code error_code;

    boost::asio::io_service* io_service;
    tcp::socket *socket;
    tcp::acceptor* acceptor;
};