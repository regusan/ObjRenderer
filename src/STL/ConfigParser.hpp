#pragma once
#include <string>
#include <map>
#include <fstream>
#include <iostream>
using namespace std;

class ConfigParser
{
private:
    map<string, string> data;

public:
    ConfigParser(string path);
    ~ConfigParser();
    void load(string path);
    float GetAsNumeric(string key);
    string GetAsString(string key);
    bool GetAsBool(string key);
    friend std::ostream &operator<<(std::ostream &os, const ConfigParser &cfg);
};
