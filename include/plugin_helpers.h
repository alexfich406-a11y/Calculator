#pragma once
#include "plugin.h"
#include <string>

class FunctionPlugin : public PluginInterface {
public:
    using Func = double (*)(double, double);

    FunctionPlugin(Type type, const char* name, int prio, Func func, bool rightAssoc = false)
        : type_(type), name_(name), prio_(prio), func_(func), rightAssoc_(rightAssoc) {}

    Type getType() const override { return type_; }
    std::string getNameFunc() const override { return name_; }
    int getPriority() const override { return prio_; }
    double calculateResult(double x, double y) const override { return func_(x, y); }
    bool isRightAssve() const override { return rightAssoc_; }

private:
    Type type_;
    std::string name_;
    int prio_;
    Func func_;
    bool rightAssoc_;
};

#define PLUGIN_DEFINE(PluginClass) extern "C" { \
    PLUGIN_API PluginInterface* createPlugin() { return new PluginClass(); } \
    PLUGIN_API void destroyPlugin(PluginInterface* p) { delete p; }  }
