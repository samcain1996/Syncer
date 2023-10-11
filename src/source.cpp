#pragma once
#include "Client.hpp"
#include "Server.hpp"

void Serve() {
    Server server;
    server.Listen();
}

void Clie() {
    Client client;
    client.Connect("192.168.50.160", "3000");
}

int main(int argc, char** argv) {

    if (argc != 2) { return -1; }

    string flag = argv[1];

    if (flag == "s") { 
        Serve();
    }
    else {
        Clie();
    }

    return 0;
}