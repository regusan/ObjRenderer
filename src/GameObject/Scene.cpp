#include "Scene.hpp"
#include "Actor.hpp"
void Scene::DestroyActor(weak_ptr<GameObject> obj)
{
    for (auto it = this->objects.begin(); it != this->objects.end(); ++it)
    {
        if (auto spobj = obj.lock(); *it == spobj)
        {
            (*it)->OnDestroyed();
            this->objects.erase(it);
            break;
        }
    }
    cerr << "不明なオブジェクトの削除を試行しました。" << endl;
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
void Scene::ExecTick(const float deltatime)
{
    for (auto &obj : this->objects)
    {
        if (obj)
            obj->Tick(deltatime);
    }
}
void Scene::ExecBeginPlay()
{
    for (auto &obj : this->objects)
    {
        if (obj)
            obj->BeginPlay();
    }
}

stringstream actorHieralcyToString(weak_ptr<Actor> actor)
{
    stringstream ss;
    if (actor.lock() == nullptr)
        return ss;
    ss << actor.lock()->name << endl;
    for (auto child : actor.lock()->children)
    {
        ss << actorHieralcyToString(child).str();
    }
    return ss;
}

stringstream Scene::hieralcyToString()
{
    stringstream ss;

    auto actors = this->GetObjectsOfClass<Actor>();
    ss << "Scene:" << actors.size() << endl;
    for (auto &actor : actors)
    {
        ss << actorHieralcyToString(actor).str();
    }
    return ss;
}