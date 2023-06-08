#pragma once

#define config std::string("/usr/local/config.ini")
#define properties std::string("/usr/local/properties.ini")

#include <string>
#include <unordered_map>

union SettingValue{
    double doubleValue;
    char*  stringValue;
    int16_t int16Value;
    bool boolValue;
};

class Parser {
public:
    bool Parse(const std::string& path);
    bool IsMcsr1();
    bool IsMcsr2();
    std::unordered_map<std::string, SettingValue> settings;
private:
    bool openFile(std::ifstream& file, const std::string& path);
    void parseConfig(std::ifstream& file);
    void parseProperties(std::ifstream& file);
    void setDefaultProperties();

private:
    bool Mcsr1Is = false;
    bool Mcsr2Is = false;
};
