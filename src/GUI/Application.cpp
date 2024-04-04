#include "GUI/Application.hpp"
#include "Networking.hpp"

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

