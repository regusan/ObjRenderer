#include "Scene.hpp"

#include "GameObject.hpp"
#include "Actor.hpp"

namespace REngine
{
    using namespace REngine::Terminal;
    void Scene::DestroyObject(weak_ptr<GameObject> obj)
    {
        if (auto spobj = obj.lock())
        {
            auto prevSize = objects.size();
            spobj->OnDestroyed();
            objects.erase(
                remove_if(objects.begin(), objects.end(),
                          [&](const shared_ptr<GameObject> &o)
                          {
                              return o == spobj;
                          }),
                objects.end());

            if (objects.size() == prevSize)
            {
                cerr << "不明なオブジェクト(" << spobj->name << ")の削除を試行しましたが、GCに存在しません。" << endl;
            }
        }
        else
        {
            cerr << "無効なオブジェクトの削除を試行しました。" << endl;
        }
    }
    void Scene::loadScene(json sceneJson)
    {
        this->objects.clear();
        for (auto &[actorName, actorData] : sceneJson["GameObjects"].items())
        {

            string classType = actorData["class"];
            json args = actorData["args"];
            // cout << actorData << endl;

            // 指定型でオブジェクトを作成
            shared_ptr<GameObject> obj = GameObjectFactory::createObject(classType, actorName, args);
            if (obj)
            {
                obj->SetSpawnedScene(this);
                obj->name = actorName;
                obj->uuid = rand();
                this->newobjects.push_back(obj);
                objects.push_back(move(obj));
            }
        }
    }
    void Scene::loadScene(filesystem::path scenepath)
    {
        // ファイルを開く
        std::ifstream file(scenepath);

        if (!file)
        {
            std::cerr << "Failed to open file " << scenepath << std::endl;
            exit(1);
        }

        // ファイルの内容を文字列に読み込む
        std::stringstream buffer;
        buffer << file.rdbuf();

        // 文字列に変換
        std::string content = buffer.str();

        // ファイルを閉じる
        file.close();
        auto jobj = nlohmann::json::parse(content);
        this->loadScene(jobj);
        this->fileWatcher = FileWatcher(scenepath, std::bind(&Scene::OnFileChanged, this, std::placeholders::_1));
        this->fileWatcher.start();
    }
    void Scene::OnFileChanged(const std::filesystem::__cxx11::path &filepath)
    {
        this->loadScene(filepath);
    }
    float Scene::ExecTick()
    {
        float deltatime = this->timeManager.Tick();
        for (auto &obj : this->objects)
        {
            if (obj)
                obj->Tick(deltatime);
        }
        return deltatime;
    }
    void Scene::ExecBeginPlay()
    {
        for (auto &obj : this->newobjects)
        {
            if (auto unlocked = obj.lock())
                unlocked->BeginPlay();
        }
        this->newobjects.clear();
    }
    string InsertAfterNewline(const string &input, const string &insertStr)
    {
        string result;
        for (size_t i = 0; i < input.size(); ++i)
        {
            result += input[i];
            if (input[i] == '\n')
            {
                result += insertStr; // 改行の直後に挿入
            }
        }
        return result;
    }
    stringstream objectHieralcyToString(weak_ptr<Actor> gameobjects, int depth = 0)
    {
        stringstream ss;
        auto unlocked = gameobjects.lock();
        if (!unlocked)
            return ss;
        stringstream tab;
        for (int i = 0; i < depth; i++)
            tab << "│\t";

        stringstream temp;
        temp << *unlocked;
        ss << tab.str() << "├──" << InsertAfterNewline(temp.str(), tab.str() + "│") << endl;

        for (auto child : unlocked->children)
        {
            ss << objectHieralcyToString(child, depth + 1).str();
        }
        return ss;
    }

    stringstream Scene::hieralcyToString()
    {
        stringstream ss;

        auto gameobjects = this->GetObjectsOfClass<GameObject>();
        auto actors = this->GetObjectsOfClass<Actor>();
        ss << C_YELLOW << "Scene:" << C_CYAN << gameobjects.size() << C_RESET << endl;
        ss << "├──" << C_YELLOW << "Timers:" << C_CYAN << timeManager.GetAllTimers().size() << C_RESET;
        ss << "(" << C_CYAN << std::fixed << std::setprecision(3) << this->timeManager.GetDeltatime() << C_RESET << "ms:";
        ss << C_CYAN << std::fixed << std::setprecision(3) << 1.0f / this->timeManager.GetDeltatime() << C_RESET << "fps)" << endl;

        ss << "├──" << C_YELLOW << "GameObjects:" << C_CYAN << gameobjects.size() << C_RESET << endl;
        for (auto &actor : actors)
        {
            if (auto unlocked = actor.lock())
                if (!unlocked->parent.lock())
                    ss << objectHieralcyToString(unlocked, 1).str();
        }
        return ss;
    }
}