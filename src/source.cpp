
#include "Client.hpp"
#include "Server.hpp"

string GetIpAddress() {
    string ip = "0.0.0.0";
    string delim = "ip=";
    ifstream inputStream("res/syncer.config");

    string line;
    while (getline(inputStream, line)) {
        if (line.find(delim) != line.npos) {
            ip = line.substr(line.find(delim) + delim.size());
            break;
        }
    }

    return ip;
}

string GetPort() {
    string port = "0";
    string delim = "port=";
    ifstream inputStream("res/syncer.config");

    string line;
    while (getline(inputStream, line)) {
        if (line.find(delim) != line.npos) {
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

void UploadClient() {
    Client client;
    if (!client.Connect(GetIpAddress(), GetPort())) {
        cerr << "Failed to connect\n";
        return;
    }

    client.GetFile(false, "NewTest");
    
    
}

void DownloadClient(string filename) {
    Client client;
    if (!client.Connect(GetIpAddress(), GetPort())) {
        cerr << "Failed to connect\n";
        return;
    }

    File file = client.GetFile(true, filename);
    if (file != NoFile) {
        for (auto x : file.value().second) {
            cout << x;
        }
    }
    cout << "\n";

}

int main(int argc, char** argv) {

    if (argc <= 1) { return -1; }

    string flag = argv[1];

    if (flag == "s") { 
        StartServer();
    }
    else if (flag == "d") {
        string filename = argv[2];
        DownloadClient(filename);
    } 
    else {
        UploadClient();
    }

    return 0;
}