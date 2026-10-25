#pragma once
#include <string>


#if defined(_WIN32)
  #define PLUGIN_API __declspec(dllexport)
#else
  #define PLUGIN_API __attribute__((visibility("default")))
#endif

enum class Type { BIN, UNO };

namespace priority {
    constexpr int Function = 0;
    constexpr int AddSub   = 10;   // + -
    constexpr int MulDiv   = 20;   // * /
    constexpr int Unary    = 30;   // neg (унарный минус): -2^2 == -4
    constexpr int Power    = 40;   // ^
}

class PluginInterface {
public:
    virtual ~PluginInterface() = default;
    virtual Type getType() const = 0;
    virtual std::string getNameFunc() const = 0;
    virtual int getPriority() const = 0;
    // Для унарных операций хост передаёт y == 0.
    virtual double calculateResult(double x, double y) const = 0;
    virtual bool isRightAssve() const { return false; }
};

constexpr const char* kCreateSymbol     = "createPlugin";
constexpr const char* kDestroySymbol    = "destroyPlugin";

using CreatePluginFn = PluginInterface* (*)();
using DestroyPluginFn = void (*)(PluginInterface*);
