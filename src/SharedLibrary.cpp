#include "../include/SharedLibrary.h"
#include "../include/CalculatorException.h"

#ifdef _WIN32
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    #ifndef NOMINMAX
        #define NOMINMAX
    #endif
    #include <windows.h>
    #include <system_error>
#else
    #include <dlfcn.h>
#endif

namespace {
#ifdef _WIN32
void* openLibrary(const std::filesystem::path& path, std::string& error) {
    HMODULE h = LoadLibraryW(path.c_str());
    if (!h) // причину читаем СРАЗУ, пока её не затёрли другие вызовы
        error = std::system_category().message(static_cast<int>(GetLastError()));
    return h;
}
void* findSymbol(void* h, const char* name) {
    return reinterpret_cast<void*>(GetProcAddress(static_cast<HMODULE>(h), name));
}
void closeLibrary(void* h) { FreeLibrary(static_cast<HMODULE>(h)); }
#else
void* openLibrary(const std::filesystem::path& path, std::string& error) {

    void* h = dlopen(path.c_str(), RTLD_NOW | RTLD_LOCAL);
    if (!h) {
        const char* e = dlerror(); 
        error = e ? e : "unknown error";
    }
    return h;
}
void* findSymbol(void* h, const char* name) { return dlsym(h, name); }
void closeLibrary(void* h) { dlclose(h); }
#endif
} 

SharedLibrary::SharedLibrary(const std::filesystem::path& path) : handle(nullptr, closeLibrary) {
    std::string error;
    void* h = openLibrary(path, error);
    if (!h)
        throw PluginException("Failed to load '" + path.string() + "': " + error);
    handle.reset(h);
}

void* SharedLibrary::getSymbol(const std::string& symbolName) const  {
    return handle ? findSymbol(handle.get(), symbolName.c_str()) : nullptr;
}
