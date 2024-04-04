#pragma once

#include <array>
#include <vector>
#include <string>
#include <optional>
#include <iomanip>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <thread>
#include <iostream>
#include <ranges>
#include <string_view>
#include <cctype>

using std::string;
using std::getline;
using std::make_unique;
using std::fstream;
using std::copy;
using std::ifstream;
using std::ofstream;
using std::ios_base;
using std::to_string;
using std::ranges::for_each;
using std::stringstream;
using std::ostringstream;
using std::istringstream;
using std::thread;
using std::cout;
using std::cerr;
using std::clog;

using std::string;

string ExtractConfig(string configName, const string& defaultValue="");

string GetIpAddress();
string GetPort();

string GetArchiveFolder();
string GetSaveFolder();