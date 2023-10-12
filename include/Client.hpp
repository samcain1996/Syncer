#pragma once
#include "Defs.hpp"

struct Client {

    Connection connection;

    Client();
    ~Client();

    bool Connect(const string& address, const string& port);

    File GetFile(const string& filename);

};