#include "../include/PluginLoader.h"
#include "../include/LoadedPlugin.h"
#include <exception>
#include <utility>

namespace fs = std::filesystem;

namespace {
// Расширение файлов плагинов в одном месте (должно совпадать с настройкой в CMake)
const char* pluginExtension() {
#if defined(_WIN32)
    return ".dll";
#elif defined(__APPLE__)
    return ".dylib";
#else
    return ".so";
#endif
}
} // namespace

PluginLoader::PluginLoader(listOperators& registry, fs::path pluginsDir)
    : registry_(registry), dir_(std::move(pluginsDir)) {}

std::map<fs::path, PluginLoader::Snapshot> PluginLoader::scan() const {
    std::map<fs::path, Snapshot> result;
    std::error_code ec;

    fs::directory_iterator it(dir_, ec), end;
    if (ec) return result;

    for (; it != end; it.increment(ec)) {
        if (ec) break;
        const fs::directory_entry& entry = *it;

        std::error_code e1, e2, e3;
        if (!entry.is_regular_file(e1) || e1) continue;
        if (entry.path().extension() != pluginExtension()) continue;

        Snapshot snap;
        snap.size = entry.file_size(e2);
        snap.mtime = entry.last_write_time(e3);
        if (e2 || e3) continue;

        result.emplace(entry.path(), snap);
    }
    return result;
}

void PluginLoader::loadOne(const fs::path& file, const Snapshot& snap, std::vector<std::string>& report) {
    const std::string fileName = file.filename().string();
    try {
        auto plugin = LoadedPlugin::load(file);
        std::string name = plugin->getNameFunc();

        if (!registry_.addPlugin(std::move(plugin))) {
            report.push_back("[plugins] " + fileName + ": operation '" + name + "' already exists, skipped");
            failed_[file] = Failed{snap, true};
            return;
        }
        loaded_[file] = Loaded{snap, name};
        report.push_back("[plugins] loaded '" + name + "' from " + fileName);
    } catch (const std::exception& e) {
        report.push_back("[plugins] " + fileName + ": " + e.what());
        failed_[file] = Failed{snap, false};
    } catch (...) {
        report.push_back("[plugins] " + fileName + ": unknown error");
        failed_[file] = Failed{snap, false};
    }
}

std::vector<std::string> PluginLoader::refresh()  {
    std::vector<std::string> report;
    try {
        const auto current = scan();

        // 1. Выгрузить исчезнувшие и изменённые файлы
        bool unloaded = false;
        for (auto it = loaded_.begin(); it != loaded_.end();) {
            auto cur = current.find(it->first);
            if (cur != current.end() && cur->second == it->second.snapshot) {
                ++it;
                continue;
            }
            registry_.removePlugin(it->second.name);
            report.push_back("[plugins] unloaded '" + it->second.name + "'" +
                             (cur != current.end() ? " (file changed)" : " (file removed)"));
            it = loaded_.erase(it);
            unloaded = true;
        }

        for (auto it = failed_.begin(); it != failed_.end();) {
            auto cur = current.find(it->first);
            bool forget = cur == current.end() || cur->second != it->second.snapshot ||
                          (unloaded && it->second.nameConflict);
            if (forget) it = failed_.erase(it);
            else ++it;
        }

        for (const auto& [file, snap] : current) {
            if (loaded_.count(file) || failed_.count(file)) continue;
            loadOne(file, snap, report);
        }
    } catch (const std::exception& e) {
        try { report.push_back(std::string("[plugins] refresh failed: ") + e.what()); } catch (...) {}
    } catch (...) {
    }
    return report;
}
