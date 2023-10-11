#pragma once
#include <unordered_map>
#include "defs.hpp"

using FileMap = std::unordered_map<string, Data>;

struct Server {
    Connection connection;

    int port;
    FileMap files;
    
    Server();
    ~Server();

    bool Listen();
    //bool UpdateFile(const string& filename, Data& data);
    //bool AddFile(const string& filename, Data& data);
    
    //File GetFile(const string& filename);
};

