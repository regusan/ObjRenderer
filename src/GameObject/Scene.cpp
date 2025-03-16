#include "Scene.hpp"
#include "Actor.hpp"
using namespace REngine::Terminal;

void Scene::DestroyActor(weak_ptr<GameObject> obj)
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
    for (auto &obj : this->objects)
    {
        if (obj)
            obj->BeginPlay();
    }
}

stringstream actorHieralcyToString(weak_ptr<Actor> actor, int depth = 0)
{
    stringstream ss;
    auto wpActor = actor.lock();
    if (!wpActor)
        return ss;

    for (int i = 0; i < depth; i++)
    {
        ss << "│\t";
    }
    ss << "├─" << C_RED << wpActor->name << C_RESET << "(" << C_BLUE << wpActor << C_RESET << ")" << endl;
    for (auto child : wpActor->children)
    {
        ss << actorHieralcyToString(child, depth + 1).str();
    }
    return ss;
}

stringstream Scene::hieralcyToString()
{
    stringstream ss;

    auto actors = this->GetObjectsOfClass<Actor>();
    auto gameobjects = this->GetObjectsOfClass<GameObject>();
    ss << C_YELLOW << "Scene:" << C_CYAN << actors.size() << C_RESET << endl;
    ss << "├─" << C_YELLOW << "Timers:" << C_CYAN << timeManager.GetAllTimers().size() << C_RESET;
    ss << "(" << C_CYAN << std::fixed << std::setprecision(3) << this->timeManager.GetDeltatime() << C_RESET << "ms:";
    ss << C_CYAN << std::fixed << std::setprecision(3) << 1.0f / this->timeManager.GetDeltatime() << C_RESET << "fps)" << endl;
    ss << "├─" << C_YELLOW << "GameObjects(System):" << C_CYAN << gameobjects.size() - actors.size() << C_RESET << endl;
    for (auto gameobject : gameobjects)
    {
        if (auto spGameObject = gameobject.lock())
        {
            if (!dynamic_pointer_cast<Actor>(spGameObject))
            {
                ss << "|" << C_RED << spGameObject->name << C_RESET << endl;
            }
        }
    }
    ss << "├─" << C_YELLOW << "Actors:" << C_CYAN << gameobjects.size() - actors.size() << C_RESET << endl;
    for (auto &actor : actors)
    {
        if (auto spactor = actor.lock())
            if (!spactor->parent.lock())
                ss << actorHieralcyToString(actor, 1).str();
    }
    return ss;
}