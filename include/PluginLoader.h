#pragma once
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <map>
#include <string>
#include <vector>
#include "listOperators.h"

class PluginLoader {
public:
    PluginLoader(listOperators& registry, std::filesystem::path pluginsDir);

    std::vector<std::string> refresh() ;

    std::size_t loadedCount() const  { return loaded_.size(); }
    const std::filesystem::path& directory() const  { return dir_; }

private:
    struct Snapshot {                      
        std::uintmax_t size = 0;
        std::filesystem::file_time_type mtime{};
        bool operator==(const Snapshot& o) const { return size == o.size && mtime == o.mtime; }
        bool operator!=(const Snapshot& o) const { return !(*this == o); }
    };
    struct Loaded {                         
        Snapshot snapshot;
        std::string name;                  
    };

    std::map<std::filesystem::path, Snapshot> scan() const;
    void loadOne(const std::filesystem::path& file, const Snapshot& snap, std::vector<std::string>& report);

    listOperators& registry_;
    std::filesystem::path dir_;
    std::map<std::filesystem::path, Loaded> loaded_;
    struct Failed {                         
        Snapshot snapshot;
        bool nameConflict = false;         
    };
    std::map<std::filesystem::path, Failed> failed_;
};
