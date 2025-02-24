#pragma once
#include <iostream>
#include <fstream>
#include <memory>
#include <vector>
#include <unordered_map>
#include <functional>
#include <nlohmann/json.hpp>

// 前方宣言（GameObject クラスが未定義のため）
class GameObject;

using json = nlohmann::json;
using namespace std;

/// @brief ランタイムでのクラス生成管理クラス
class GameObjectFactory
{
public:
    /// @brief 一次的なエイリアス。名前と引数を受け取る関数型
    using ObjectCreateFunc = function<unique_ptr<GameObject>(const string &, const json &)>;

    /// @brief クラス登録のためのマップを取得
    static unordered_map<string, ObjectCreateFunc> &getRegistry()
    {
        static unordered_map<string, ObjectCreateFunc> registry;
        return registry;
    }

    /// @brief クラスをファクトリに登録
    static void registerObject(const string &className, ObjectCreateFunc createFunc)
    {
        getRegistry()[className] = move(createFunc);
    }

    /// @brief クラス名からオブジェクトを生成
    static unique_ptr<GameObject> createObject(const string &className, const string &name, const json &params)
    {
        auto &registry = getRegistry();
        if (registry.find(className) != registry.end())
        {
            return registry[className](name, params);
        }
        cerr << "Error: Unknown class type '" << className << "'\n";
        return nullptr;
    }
};

// 指定型クラスについて、文字列での生成に対応させるマクロ。引き数Json。
#define GCLASS(ClassType)                                                                               \
    class ClassType##Registrar                                                                          \
    {                                                                                                   \
    private:                                                                                            \
        static int registerClass()                                                                      \
        {                                                                                               \
            GameObjectFactory::registerObject(                                                          \
                #ClassType,                                                                             \
                [](const string &name, const json &params) { return make_unique<ClassType>(params); }); \
            return 0;                                                                                   \
        }                                                                                               \
        static inline int dummy = registerClass();                                                      \
    };

// 指定型クラスについて、文字列での生成に対応させるマクロ。引き数無し。
#define GCLASS_NO_ARG(ClassType)                                                                  \
    class ClassType##Registrar                                                                    \
    {                                                                                             \
    private:                                                                                      \
        static int registerClass()                                                                \
        {                                                                                         \
            GameObjectFactory::registerObject(                                                    \
                #ClassType,                                                                       \
                [](const string &name, const json &params) { return make_unique<ClassType>(); }); \
            return 0;                                                                             \
        }                                                                                         \
        static inline int dummy = registerClass();                                                \
    };

// マクロ終了