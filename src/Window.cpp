#include "Window.hpp"

wxBEGIN_EVENT_TABLE(StartWindow, wxFrame)
    EVT_BUTTON(StartWindow::START_SERVER_BUTTON_ID, StartWindow::HandleStartServer)
    EVT_BUTTON(StartWindow::CONNECT_TO_SERVER_BUTTON_ID, StartWindow::HandleConnectToServer)
wxEND_EVENT_TABLE()

StartWindow::StartWindow() : wxFrame(nullptr, wxID_ANY, "Syncer", wxPoint(100, 100), wxSize(800, 600)) {

    startServerButton = new wxButton(this, START_SERVER_BUTTON_ID, "Start Server", wxPoint(100, 250), wxSize(250, 30));
    connectToServerButton = new wxButton(this, CONNECT_TO_SERVER_BUTTON_ID, "Connect To Server", wxPoint(400, 250), wxSize(250, 30));
}

void StartWindow::HandleStartServer(wxCommandEvent& evt) {
    startServerButton->Enable(false);
    connectToServerButton->Enable(false);

    thread(StartServer).detach();
}

void StartWindow::HandleConnectToServer(wxCommandEvent& evt) {

    ClientWindow* clientWindow = new ClientWindow;
    clientWindow->Show(true);
    Close(true);

}

wxBEGIN_EVENT_TABLE(ClientWindow, wxFrame)
    EVT_BUTTON(10001, ClientWindow::HandleDownload)
    EVT_BUTTON(10002, ClientWindow::HandleUpload)
wxEND_EVENT_TABLE()

ClientWindow::ClientWindow() : wxFrame(nullptr, wxID_ANY, "Syncer", wxPoint(100, 100), wxSize(800, 600)) {

    savedFilesList = new wxListBox(this, 10003, wxPoint(100, 50), wxSize(600, 150));
    downloadButton = new wxButton(this, 10001, "Download", wxPoint(100, 250), wxSize(250, 30));
    uploadButton = new wxButton(this, 10002, "Upload", wxPoint(400, 250), wxSize(250, 30));
    uploadFileName = new wxTextCtrl(this, 10004, "", wxPoint(250, 350), wxSize(250, 30));

    PopulateListBox();
}

void ClientWindow::PopulateListBox() {

    savedFilesList->Clear();

    stringstream files(ListServerFiles());
    string filename;
    wxArrayString savedFiles;
    while (getline(files, filename)) {
        savedFiles.Add(filename);
    }

    savedFilesList->InsertItems(savedFiles, 0);
}

void ClientWindow::HandleDownload(wxCommandEvent& evt) {
    string filename { savedFilesList->GetString(savedFilesList->GetSelection()).c_str() };
    DownloadClient(filename);
}

void ClientWindow::HandleUpload(wxCommandEvent& evt) {
    UploadClient(string { uploadFileName->GetValue().c_str() } );
    PopulateListBox();
}

ServerWindow::ServerWindow() {}