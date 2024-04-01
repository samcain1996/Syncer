#include "wx/wx.h"

class StartWindow : public wxFrame {

    constexpr static const int START_SERVER_BUTTON_ID       = 10001;
    constexpr static const int CONNECT_TO_SERVER_BUTTON_ID  = 10002;

    wxButton*   startServerButton       = nullptr;
    wxButton*   connectToServerButton   = nullptr;
    wxListBox*  savedFilesList          = nullptr;
    
    void HandleStartServer(wxCommandEvent& evt);
    void HandleConnectToServer(wxCommandEvent& evt);

public:
    StartWindow();

    wxDECLARE_EVENT_TABLE();
};

class Application : public wxApp {
public:
    Application();
    ~Application();

private:
    StartWindow* _window = nullptr;

public:
    virtual bool OnInit() wxOVERRIDE ;
};

