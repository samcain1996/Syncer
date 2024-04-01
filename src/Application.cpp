#include "Application.hpp"
#include "Networking.hpp"

using Args = std::vector<std::string>;

using std::cout;
using std::cerr;
using std::clog;

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

wxBEGIN_EVENT_TABLE(StartWindow, wxFrame)
    EVT_BUTTON(StartWindow::START_SERVER_BUTTON_ID, StartWindow::HandleStartServer)
    EVT_BUTTON(StartWindow::CONNECT_TO_SERVER_BUTTON_ID, StartWindow::HandleConnectToServer)
wxEND_EVENT_TABLE()

StartWindow::StartWindow() : wxFrame(nullptr, wxID_ANY, "Syncer", wxPoint(100, 100), wxSize(800, 600)) {

    startServerButton = new wxButton(this, START_SERVER_BUTTON_ID, "Start Server", wxPoint(100, 250), wxSize(250, 30));
    connectToServerButton = new wxButton(this, CONNECT_TO_SERVER_BUTTON_ID, "Connect To Server", wxPoint(400, 250), wxSize(250, 30));
    savedFilesList = new wxListBox(this, 10003, wxPoint(100, 50), wxSize(600, 150));
}

void StartWindow::HandleStartServer(wxCommandEvent& evt) {
    startServerButton->Enable(false);
    connectToServerButton->Enable(false);

    thread([this](){ 
        
        StartServer(); 

        startServerButton->Enable(true);
        connectToServerButton->Enable(true);
    
    }).detach();
}

void StartWindow::HandleConnectToServer(wxCommandEvent& evt) {
    startServerButton->Enable(false);
    connectToServerButton->Enable(false);

    thread([this](){

        stringstream files(ListServerFiles());

        string filename;
        wxArrayString savedFiles;

        while (getline(files, filename)) {
            savedFiles.Add(filename);
        }

        savedFilesList->InsertItems(savedFiles, 0);
        
        startServerButton->Enable(true);
        connectToServerButton->Enable(true);

    }).detach();
}


Application::Application() {}
Application::~Application() {}

bool Application::OnInit() {

    Connection::SAVE_FOLDER    = GetSaveFolder();
    Connection::ARCHIVE_FOLDER = GetArchiveFolder();

    _window = new StartWindow();
    _window->Show();

    return true;
}

