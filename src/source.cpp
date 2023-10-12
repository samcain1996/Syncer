
#include "Client.hpp"
#include "Server.hpp"

string GetIpAddress() {
    string ip = "0.0.0.0";
    string delim = "ip=";
    ifstream inputStream("res/Client.config");

    string line;
    while (std::getline(inputStream, line)) {
        if (line.find("ip=") != line.npos) {
            ip = line.substr(line.find(delim) + delim.size());
            break;
        }
    }

    return ip;
}

string GetPort() {
    string port = "0";
    string delim = "port=";
    ifstream inputStream("res/Client.config");

    string line;
    while (std::getline(inputStream, line)) {
        if (line.find("port=") != line.npos) {
            port = line.substr(line.find(delim) + delim.size());
            break;
        }
    }

    return port;
}

void StartServer() {
    Server server(GetPort());
    if (!server.Listen()) {
        cerr << "Failed to connect\n";
        return;
    }
    
    server.Loop();

}

void StartClient() {
    Client client;
    if (!client.Connect(GetIpAddress(), GetPort())) {
        cerr << "Failed to connect\n";
        return;
    }
    
    auto file = client.GetFile("Test");
    for (int i = 0; i < file.value().second.size(); i++) {
        cout << file.value().second[i];
    }
    cout << "\n";
}

int main(int argc, char** argv) {

    if (argc != 2) { return -1; }

    string flag = argv[1];

    if (flag == "s") { 
        StartServer();
    }
    else {
        StartClient();
    }

    return 0;
}