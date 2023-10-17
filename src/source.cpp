
#include <iostream>
#include "Networking.hpp"

using Args = std::vector<std::string>;

using std::cout;
using std::cerr;

string ExtractConfig(string configName, const string& defaultValue="") {

    string config = defaultValue;
    if (configName.back() != '=') { configName += "="; }
    fstream inputStream("res/syncer.config", std::ios_base::in | std::ios_base::binary);

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

string GetArchiveFolder() {
    return ExtractConfig("archive_folder", "archived");
}

string GetSaveFolder() {
    return ExtractConfig("save_folder", "saved");
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
        std::ofstream f(filename);
        f.write(file.value().second.data(), file.value().second.size());
    }

}

int main(int argc, char** argv) {

    if (argc <= 1) { return -1; }

    Args args;
    for (int i = 1; i < argc; ++i) {
        args.push_back(argv[i]);
    }

    Connection::SAVE_FOLDER    = GetSaveFolder();
    Connection::ARCHIVE_FOLDER = GetArchiveFolder();

    string command = args[0];

    if (command == "s") { 
        StartServer();
    }
    else if (command == "d") {
        DownloadClient(args[1]);
    } 
    else if (command == "u") {
        UploadClient(args[1]);
    }
    else {
        cerr << "Invalid command\n";
        return -2;
    }

    return 0;
}