#include "GUI/Application.hpp"
#include "Networking.hpp"

using Args = std::vector<std::string>;

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

string ListServerFiles() {

    Client client;
    if (!client.Connect(GetIpAddress(), GetPort())) {
        cerr << "Failed to connect\n";
        return "Error";
    }

    return client.GetContents();
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


Application::Application() {}
Application::~Application() {}

bool Application::OnInit() {

    wxInitAllImageHandlers();

    Connection::SAVE_FOLDER    = GetSaveFolder();
    Connection::ARCHIVE_FOLDER = GetArchiveFolder();

    _window = new StartWindow();
    _window->Show();

    return true;
}

