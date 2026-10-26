#include "../include/ExecutablePath.h"
#include <system_error>
#include <vector>

#if defined(_WIN32)
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    #ifndef NOMINMAX
        #define NOMINMAX
    #endif
    #include <windows.h>
#elif defined(__APPLE__)
    #include <mach-o/dyld.h>
#endif

namespace fs = std::filesystem;

fs::path executableDir(const char* argv0) {
    fs::path exe;
    std::error_code ec;

#if defined(_WIN32)
    std::vector<wchar_t> buf(32768);
    DWORD n = GetModuleFileNameW(nullptr, buf.data(), static_cast<DWORD>(buf.size()));
    if (n > 0 && n < buf.size()) exe = fs::path(std::wstring(buf.data(), n));
#elif defined(__APPLE__)
    uint32_t size = 0;
    _NSGetExecutablePath(nullptr, &size); 
    std::vector<char> buf(size + 1);
    if (_NSGetExecutablePath(buf.data(), &size) == 0) exe = fs::weakly_canonical(buf.data(), ec);
#else
    exe = fs::read_symlink("/proc/self/exe", ec);
#endif

    if (exe.empty() && argv0 && *argv0)
        exe = fs::absolute(argv0, ec);
    return exe.has_parent_path() ? exe.parent_path() : fs::current_path(ec);
}
