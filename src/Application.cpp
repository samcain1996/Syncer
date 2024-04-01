#include "Application.hpp"


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

wxIMPLEMENT_APP(Application);

StartWindow::StartWindow() : wxFrame(nullptr, wxID_ANY, "Syncer", wxPoint(100, 100), wxSize(800, 600)) {

    Connection::SAVE_FOLDER    = GetSaveFolder();
    Connection::ARCHIVE_FOLDER = GetArchiveFolder();

    portTb = new wxTextCtrl(this, wxID_ANY, GetPort(), wxPoint(200, 100), wxSize(400, 25));
    save_folderTb = new wxTextCtrl(this, wxID_ANY, "./" + GetSaveFolder(), wxPoint(200, 200), wxSize(400, 25));
    archive_folderTb = new wxTextCtrl(this, wxID_ANY, "./" + GetArchiveFolder(), wxPoint(200, 300), wxSize(400, 25));

    startServerButton = new wxButton(this, wxID_ANY, "Start Server", wxPoint(100, 400), wxSize(100, 30));
    connectToServerButton = new wxButton(this, wxID_ANY, "Connect To Server", wxPoint(400, 400), wxSize(100, 30));
}

Application::Application() {}
Application::~Application() {}

bool Application::OnInit() {

    _window = new StartWindow();
    _window->Show();
    return true;
}

