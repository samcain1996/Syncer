#include "Networking.hpp"

extern void StartServer();
extern void DownloadClient(string);
extern void UploadClient(string);
extern string ListServerFiles();

class StartWindow : public wxFrame {

    constexpr static const int START_SERVER_BUTTON_ID       = 10001;
    constexpr static const int CONNECT_TO_SERVER_BUTTON_ID  = 10002;

    wxButton*   startServerButton       = nullptr;
    wxButton*   connectToServerButton   = nullptr;
    
    void HandleStartServer(wxCommandEvent& evt);
    void HandleConnectToServer(wxCommandEvent& evt);

public:
    StartWindow();

    wxDECLARE_EVENT_TABLE();
};

class ClientWindow : public wxFrame {

    void PopulateListBox();

public:
    ClientWindow();

    wxButton* downloadButton    = nullptr;
    wxButton* uploadButton      = nullptr;
    wxListBox* savedFilesList   = nullptr;
    wxTextCtrl* uploadFileName  = nullptr;

    void HandleDownload(wxCommandEvent& evt);
    void HandleUpload(wxCommandEvent& evt);

    wxDECLARE_EVENT_TABLE();
};
