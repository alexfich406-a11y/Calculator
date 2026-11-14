#pragma once
#include <map>
#include <memory>
#include <string>
#include "plugin.h"

class listOperators {
    std::map<std::string, std::unique_ptr<PluginInterface>> ext_plugins;
    std::map<std::string, std::unique_ptr<PluginInterface>> basic_operations;

public:
    const PluginInterface* findOperation(const std::string& name) const;

    bool isPlugin(const std::string& name) const;
    bool isBasicOp(const std::string& name) const;
    bool containsOp(const std::string& name) const;

    int getPriority(const std::string& name) const;
    Type getType(const std::string& name) const;
    bool isRightAssve(const std::string& name) const;

    void addBasic(std::unique_ptr<PluginInterface> op);

    bool addPlugin(std::unique_ptr<PluginInterface> op);

    bool removePlugin(const std::string& name);
};
