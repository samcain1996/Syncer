#pragma once

#include "Window.hpp"

class Application : public wxApp {
public:
    Application();
    ~Application();

private:
    StartWindow* _window = nullptr;

public:
    virtual bool OnInit() wxOVERRIDE ;
};

