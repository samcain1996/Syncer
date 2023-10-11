
#include "Client.hpp"
#include "Server.hpp"

void Serve() {
    Server server;
    if (!server.Listen()) {
        cerr << "Failed to connect";
    }
    else { cout << "Connected to Client!"; }
}

void Clie() {
    Client client;
    if (!client.Connect("192.168.50.130", "3000")) {
        cerr << "Failed to connect";
    }
    else { cout << "Connected to Server!"; }
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