#pragma once
#include "Defs.hpp"

struct Client {

    Connection connection;

    Client();
    ~Client();

    bool Connect(const string& address, const string& port);
    //bool UpdateFile(const string& filename, Data& data);
    //bool AddFile(const string& filename, Data& data);

    File GetFile(const string& filename) const;

};