#pragma once
#include "Defs.hpp"

struct Server {
    Connection connection;
    
    Server(const string& port);
    ~Server();

    bool Listen();
    void Loop();
    bool AddFile(const string& filename, Data& data);
    
    bool SendFile(const string& filename);
};

