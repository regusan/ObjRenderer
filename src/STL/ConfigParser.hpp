#pragma once
#include <string>
#include <map>
#include <fstream>
#include <iostream>
using namespace std;

/// @brief コンフィグファイルから値を読み取るクラス
class ConfigParser
{
private:
    map<string, string> data;
    /// @brief ロードする内部関数
    /// @param path
    void load(string path);

public:
    /// @brief COnfigファイルからロード
    /// @param path
    ConfigParser(string path);
    ~ConfigParser();

    /// @brief 数字として値を取得
    /// @param key
    /// @return
    float GetAsNumeric(string key);
    /// @brief 文字列として値を取得
    /// @param key
    /// @return
    string GetAsString(string key);
    /// @brief ブールとして値を取得
    /// @param key
    /// @return
    bool GetAsBool(string key);

    friend std::ostream &operator<<(std::ostream &os, const ConfigParser &cfg);
};
