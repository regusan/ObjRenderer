#pragma once

#include <string>
#include <functional>
#include <filesystem>
#include <chrono>
#include <thread>

/// @brief ファイルを監視し、リロードするクラス
class FileWatcher
{

private:
    std::filesystem::path filePath;
    std::function<void(const std::filesystem::path &)> callback;
    std::filesystem::file_time_type lastWriteTime;
    bool running;

public:
    FileWatcher() {}
    FileWatcher(const std::filesystem::path &path, std::function<void(const std::filesystem::path &)> cb) : filePath(path), running(false)
    {
        if (std::filesystem::exists(filePath))
        {
            lastWriteTime = std::filesystem::last_write_time(filePath);
            callback = cb;
        }
    }

    void start()
    {
        running = true;
        std::thread([this]()
                    {
            while (running) {
                if (std::filesystem::exists(filePath)) {
                    auto currentWriteTime = std::filesystem::last_write_time(filePath);
                    if (currentWriteTime != lastWriteTime) {
                        lastWriteTime = currentWriteTime;
                        if (callback) {
                            callback(filePath);
                        }
                    }
                }
                std::this_thread::sleep_for(std::chrono::seconds(1));
            } })
            .detach();
    }

    void stop()
    {
        running = false;
    }
};