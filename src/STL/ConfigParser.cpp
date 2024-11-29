#include "ConfigParser.hpp"

ConfigParser::ConfigParser(string path)
{
    this->load(path);
}

ConfigParser::~ConfigParser()
{
}

void ConfigParser::load(string path)
{
    string line;
    data.clear();
    ifstream infile(path);
    if (!infile)
    {
        cerr << "ファイルを開けませんでした！" << std::endl;
        exit(1);
    }

    while (getline(infile, line))
    {
        string nonspace = "";
        for (char c : line)
        {
            if (c != ' ')
                nonspace.push_back(c);
        }

        if (nonspace[0] == '#') // コメントは無視
            continue;
        else if (nonspace[0] == '\n' || nonspace[0] == '\0') // 空行や行末無視
            continue;
        size_t idx = nonspace.find("=");
        string key = "";
        string value = "";
        for (size_t i = 0; i < idx; i++)
        {
            key.push_back(nonspace[i]);
        }
        for (size_t i = idx + 1; i < nonspace.size(); i++)
        {
            value.push_back(nonspace[i]);
        }
        data[key] = value;
    }
}
float ConfigParser::GetAsNumeric(string key)
{
    try
    {
        return stof(this->GetAsString(key));
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        std::cerr << "\"" << key << "\":\"" << this->GetAsString(key) << "\"は変換不可能な値" << '\n';
        exit(1);
    }
}
string ConfigParser::GetAsString(string key)
{
    if (data.find(key) != data.end())
        return data[key];

    else
    {
        std::cerr << "Key:\"" << key << "\"は存在しないキー" << '\n';
        exit(1);
    }
}
bool ConfigParser::GetAsBool(string key)
{
    return (this->GetAsString(key) == "True") || (this->GetAsString(key) == "true");
}

std::ostream &operator<<(std::ostream &os, const ConfigParser &cfg)
{
    os << "config = {" << endl;
    for (const auto &[key, value] : cfg.data)
    {
        os << "\t" << key << "\t:\t" << value << endl;
    }
    os << "}" << endl;
    return os;
}