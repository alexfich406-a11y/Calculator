#include "../include/listOperators.h"
#include "../include/CalculatorException.h"
#include <utility>

const PluginInterface* listOperators::findOperation(const std::string& name) const {
    // Встроенные операции приоритетнее плагинов
    auto b = basic_operations.find(name);
    if (b != basic_operations.end())
        return b->second.get();

    auto p = ext_plugins.find(name);
    if (p != ext_plugins.end())
        return p->second.get();

    throw CalculatorException("Unknown operation or function name: '" + name + "'");
}

bool listOperators::isPlugin(const std::string& name) const { 
    return ext_plugins.count(name) != 0; 
}
bool listOperators::isBasicOp(const std::string& name) const { 
    return basic_operations.count(name) != 0;
 }
bool listOperators::containsOp(const std::string& name) const { 
    return isBasicOp(name) || isPlugin(name); 
}

int listOperators::getPriority(const std::string& name) const { 
    return findOperation(name)->getPriority();
 }
Type listOperators::getType(const std::string& name) const { 
    return findOperation(name)->getType(); }
bool listOperators::isRightAssve(const std::string& name) const { 
    return findOperation(name)->isRightAssve();
 }

void listOperators::addBasic(std::unique_ptr<PluginInterface> op) {
    std::string name = op->getNameFunc();
    if (!basic_operations.emplace(name, std::move(op)).second)
        throw CalculatorException("Basic operation '" + name + "' is already registered");
}

bool listOperators::addPlugin(std::unique_ptr<PluginInterface> op) {
    std::string name = op->getNameFunc();
    if (containsOp(name))
        return false;
    ext_plugins.emplace(std::move(name), std::move(op));
    return true;
}

bool listOperators::removePlugin(const std::string& name) {
    return ext_plugins.erase(name) != 0;
}
