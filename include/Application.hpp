#include "Window.hpp"

class Application : public wxApp {
public:
    Application();
    ~Application();

private:
    wxFrame* _window = nullptr;

public:
    virtual bool OnInit() wxOVERRIDE ;
};

