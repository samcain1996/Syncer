#pragma once
#include <vector>
#include <iostream>
#include <string>
#include <optional>
#include <fstream>
#include <tuple>
#include <array>
#include <functional>
#include "boost/asio.hpp"

using Args = std::vector<std::string>;
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
using std::function;
using std::tuple;
using std::make_tuple;

using namespace boost::asio;
using namespace boost::asio::ip;
using namespace boost::system;

using SocketPtr     = std::unique_ptr<tcp::socket>;
using AcceptorPtr   = std::unique_ptr<tcp::acceptor>;
using IO_ServicePtr = std::unique_ptr<boost::asio::io_service>;

static inline const int BUFFER_SIZE = 2048;

static constexpr const std::nullopt_t NoFile = std::nullopt;
using Buffer = std::array<char, BUFFER_SIZE>;
