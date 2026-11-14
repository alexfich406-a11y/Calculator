#include "../include/LoadedPlugin.h"
#include "../include/CalculatorException.h"
#include <atomic>
#include <chrono>
#include <stdexcept>

namespace fs = std::filesystem;

namespace {

TempFile makeTempCopy(const fs::path& source) {
    static std::atomic<unsigned> counter{0};

    std::error_code ec;
    fs::path dir = fs::temp_directory_path(ec);
    if (ec)
        throw PluginException("No temp directory: " + ec.message());

    auto stamp = std::chrono::steady_clock::now().time_since_epoch().count();
    fs::path target = dir / ("calc_plugin_" + std::to_string(stamp) + "_" +
                             std::to_string(++counter) + source.extension().string());

    fs::copy_file(source, target, fs::copy_options::overwrite_existing, ec);
    if (ec)
        throw PluginException("Cannot copy '" + source.string() + "': " + ec.message());
    return TempFile(std::move(target));
}

template <typename Fn>
Fn symbolAs(const SharedLibrary& lib, const char* name) {
    return reinterpret_cast<Fn>(lib.getSymbol(name));
}

} 

LoadedPlugin::LoadedPlugin(TempFile&& temp, SharedLibrary&& lib, PluginPtr&& plugin)
    : tempFile_(std::move(temp)), library_(std::move(lib)), plugin_(std::move(plugin)),
      name_(plugin_->getNameFunc()), type_(plugin_->getType()),
      priority_(plugin_->getPriority()), rightAssoc_(plugin_->isRightAssve()) {
    if (name_.empty())
        throw PluginException("Plugin returned an empty operation name");
}

std::unique_ptr<LoadedPlugin> LoadedPlugin::load(const fs::path& source) {
    TempFile temp = makeTempCopy(source);


    SharedLibrary lib = [&] {
        try {
            return SharedLibrary(temp.path());
        } 
        catch (const CalculatorException& e) {
            std::string msg = e.what();
            const std::string from = temp.path().string(), to = source.string();
            for (auto p = msg.find(from); p != std::string::npos; p = msg.find(from, p + to.size()))
                msg.replace(p, from.size(), to);
            throw std::runtime_error(msg);
        }
    }();

   

    auto create  = symbolAs<CreatePluginFn>(lib, kCreateSymbol);
    auto destroy = symbolAs<DestroyPluginFn>(lib, kDestroySymbol);
    if (!create || !destroy)
        throw PluginException("Plugin must export '" + std::string(kCreateSymbol) + "' and '" +
                              std::string(kDestroySymbol) + "'");

    PluginInterface* raw = create();
    if (!raw)
        throw PluginException("Plugin factory returned nullptr");
    PluginPtr plugin(raw, destroy); 

    return std::unique_ptr<LoadedPlugin>(new LoadedPlugin(std::move(temp), std::move(lib), std::move(plugin)));
}

double LoadedPlugin::calculateResult(double x, double y) const {
    try {
        return plugin_->calculateResult(x, y);
    } catch (const std::exception& e) {
        throw PluginException("Function '" + name_ + "' threw an exception: " + e.what());
    } catch (...) {
        throw PluginException("Function '" + name_ + "' threw an unknown exception");
    }
}
