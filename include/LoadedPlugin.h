#pragma once
#include <filesystem>
#include <memory>
#include <string>
#include <system_error>
#include <utility>
#include "SharedLibrary.h"
#include "plugin.h"

class TempFile {
public:
    TempFile() = default;
    explicit TempFile(std::filesystem::path p) : path_(std::move(p)) {}
    TempFile(TempFile&& o)  : path_(std::move(o.path_)) { o.path_.clear(); }
    TempFile& operator=(TempFile&& o)  {
        if (this != &o) { remove(); path_ = std::move(o.path_); o.path_.clear(); }
        return *this;
    }
    TempFile(const TempFile&) = delete;
    TempFile& operator=(const TempFile&) = delete;
    ~TempFile() { remove(); }

    const std::filesystem::path& path() const { return path_; }

private:
    void remove()  {
        if (path_.empty()) return;
        std::error_code ec;
        std::filesystem::remove(path_, ec);
    }
    std::filesystem::path path_;
};

class LoadedPlugin final : public PluginInterface {
public:
    static std::unique_ptr<LoadedPlugin> load(const std::filesystem::path& source);

    Type getType() const override { return type_; }
    std::string getNameFunc() const override { return name_; }
    int getPriority() const override { return priority_; }
    bool isRightAssve() const override { return rightAssoc_; }

    double calculateResult(double x, double y) const override;

private:
    using PluginPtr = std::unique_ptr<PluginInterface, DestroyPluginFn>;

    LoadedPlugin(TempFile&& temp, SharedLibrary&& lib, PluginPtr&& plugin);

    TempFile tempFile_;
    SharedLibrary library_; 
    PluginPtr plugin_;  
    std::string name_;   
    Type type_;
    int priority_;
    bool rightAssoc_;
};
