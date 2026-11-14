#pragma once
#include "../include/plugin.h"
#include <string>
#include <functional>

class BasicOperation final : public PluginInterface {
private:
    Type type;
    std::string NameFunc;
    int prty;
    std::function<double(double, double)> calc_func;

public:
    BasicOperation(Type t, std::string name, int p, std::function<double(double, double)> f);

    Type getType() const override;
    std::string getNameFunc() const override;
    int getPriority() const override;
    double calculateResult(double x, double y) const override;
};
