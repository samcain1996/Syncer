#include "wx/wx.h"

class StartWindow : public wxFrame {
public:
    StartWindow();

    wxTextCtrl* portTb                  = nullptr;
    wxTextCtrl* save_folderTb           = nullptr;
    wxTextCtrl* archive_folderTb        = nullptr;

    wxButton*   startServerButton       = nullptr;
    wxButton*   connectToServerButton   = nullptr;
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

