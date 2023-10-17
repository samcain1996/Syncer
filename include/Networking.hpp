#pragma once
#include "Defs.hpp"

File ReadFile(const string& filename, const string& folder="syncedFiles/");

struct Connection {

    static inline const Data DISCONNECT_MESSAGE = { 'B', 'Y', 'E', '!' };
    static inline const Data ACK_MESSAGE        = { 'A', 'C', 'K' };

    Buffer buf;

    bool connected = false;
    IO_ServicePtr io_service;
    SocketPtr     socket;
    AcceptorPtr   acceptor;
    boost::system::error_code error_code;

    void Disconnect();
    bool SendData(const Data& data);
    bool SendData(const string& data);
    size_t ReceiveData();

    bool IsConnected() const;
    bool IsDisconnectMessage(const Buffer& message) const;

};

struct Client {

    Connection connection;

    Client();
    ~Client();

    bool Connect(const string& address, const string& port);

    File GetFile(string filename);
    bool UploadFile(string filename);

};

struct Server {
    
    Connection connection;
    
    Server(const string& port);
    ~Server();

    bool Listen();
    void Loop();
    bool AddFile(const string& filename, Data& data);
};

