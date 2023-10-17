
#include <iostream>
#include "Networking.hpp"


using Args = std::vector<std::string>;

using std::cout;
using std::cerr;

string ExtractConfig(string configName, const string& defaultValue="") {

    string config = defaultValue;
    if (configName.back() != '=') { configName += "="; }
    fstream inputStream("res/syncer.config", std::ios_base::in);

    string line;
    while (getline(inputStream, line)) {
        if (line.find(configName) != line.npos) {
            config = line.substr(line.find(configName) + configName.size());
            break;
        }
    }

    return config;

}

string GetIpAddress() {
    return ExtractConfig("ip", "0.0.0.0");
}

string GetPort() {
    return ExtractConfig("port", "0");
}

void StartServer() {
    Server server(GetPort());
    if (!server.Listen()) {
        cerr << "Failed to connect\n";
        return;
    }
    
    server.Loop();

}

void UploadClient(string filename) {
    Client client;
    if (!client.Connect(GetIpAddress(), GetPort())) {
        cerr << "Failed to connect\n";
        return;
    }

    client.UploadFile(filename);
    
    
}

void DownloadClient(string filename) {
    Client client;
    if (!client.Connect(GetIpAddress(), GetPort())) {
        cerr << "Failed to connect\n";
        return;
    }

    File file = client.GetFile(filename);
    if (file != NoFile) {
        for (auto x : file.value().second) {
            cout << x;
        }
    }
    cout << "\n";

}

int main(int argc, char** argv) {

    if (argc <= 1) { return -1; }

    Args args;
    for (int i = 1; i < argc; ++i) {
        args.push_back(argv[i]);
    }

    string command = args[0];

    if (command == "s") { 
        StartServer();
    }
    else if (command == "d") {
        DownloadClient(args[1]);
    } 
    else {
        UploadClient(args[1]);
    }

    return 0;
}