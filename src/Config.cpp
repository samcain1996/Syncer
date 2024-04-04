#include "Config.hpp"

string ExtractConfig(string configName, const string& defaultValue) {

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
    ostringstream ip;
    for (const auto digit : ExtractConfig("ip", "0.0.0.0")) {
        if (std::isdigit(digit) || digit == '.') {
            ip << digit;
        }
    }
    return ip.str();
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
