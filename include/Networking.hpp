#pragma once

#include <array>
#include <vector>
#include <string>
#include <optional>
#include <fstream>
#include "boost/asio.hpp"

using Data = std::vector<char>;
using File = std::optional<std::pair<std::string, Data>>;

using namespace boost::asio;
using namespace boost::asio::ip;
using namespace boost::system;

using SocketPtr     = std::unique_ptr<tcp::socket>;
using AcceptorPtr   = std::unique_ptr<tcp::acceptor>;
using IO_ContextPtr = std::unique_ptr<boost::asio::io_context>;

using std::string;
using std::getline;
using std::make_unique;
using std::fstream;
using std::copy;
using std::ifstream;
using std::ofstream;
using std::ios_base;
using std::to_string;

static constexpr const size_t BUFFER_SIZE = 2048;
static constexpr const std::nullopt_t NoFile = std::nullopt;

using Buffer = std::array<char, BUFFER_SIZE>;

File ReadFile(const string& filename, const string& folder="saved/");

struct Connection {

    static inline const Data DISCONNECT_MESSAGE = { 'B', 'Y', 'E', '!' };
    static inline const Data ACK_MESSAGE        = { 'A', 'C', 'K' };
    static inline string ARCHIVE_FOLDER;
    static inline string SAVE_FOLDER;

    static bool portOpen(unsigned short port);
    
    Buffer buf;
    bool connected = false;

    IO_ContextPtr io_context;
    SocketPtr     socket;
    AcceptorPtr   acceptor;
    error_code    err_code;

    bool SendData(const Data& data);
    Data ReceiveData();

    bool IsConnected() const;
    bool IsDisconnectMessage(const Buffer& message) const;
    bool HasAcknowledged() const;

};

struct Client {

    Connection connection;

    Client();

    bool Connect(const string& address, const string& port);
    File GetFile(const string& filename);
    bool UploadFile(const string& filename);

};

struct Server {
    
    Connection connection;
    
    Server(const string& port);

    bool Listen();
    void Loop();
    bool UpdateFile(const string& filename, Data& data);
    bool AddFile(const string& filename, Data& data);
};

