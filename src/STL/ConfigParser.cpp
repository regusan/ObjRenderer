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
        if (line[0] == '#') // コメントは無視
            continue;
        else if (line[0] == ' ' || line[0] == '\n' || line[0] == '\0') // 空白無視
            continue;
        size_t idx = line.find("=");
        string key = "";
        string value = "";
        for (size_t i = 0; i < idx; i++)
        {
            key.push_back(line[i]);
        }
        for (size_t i = idx + 1; i < line.size(); i++)
        {
            value.push_back(line[i]);
        }
        data[key] = value;
    }
}
float ConfigParser::GetAsNumeric(string key)
{
    return stof(data[key]);
}
string ConfigParser::GetAsString(string key)
{
    return data[key];
}
bool ConfigParser::GetAsBool(string key)
{
    return (data[key] == "True") || (data[key] == "true");
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