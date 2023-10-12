#pragma once
#include <unordered_map>
#include "Defs.hpp"

using FileMap = std::unordered_map<string, Data>;

struct Server {
    Connection connection;

    int port;
    FileMap files;
    
    Server();
    ~Server();

    bool Listen();
    void Loop();
    bool InitFiles();
    bool AddFile(const string& filename, Data& data);
    
    File SendFile(const string& filename);
};

