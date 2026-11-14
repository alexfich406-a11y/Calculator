#pragma once
#include <filesystem>
#include <memory>
#include <string>


class SharedLibrary final {
    using LibDeleter = void (*)(void*);
    std::unique_ptr<void, LibDeleter> handle;

public:

SharedLibrary(const std::filesystem::path& path);
    void* getSymbol(const std::string& symbolName) const ;
};
