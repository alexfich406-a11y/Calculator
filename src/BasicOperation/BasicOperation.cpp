#include "BasicOperation.h"
#include <utility>

BasicOperation::BasicOperation(Type t, std::string name, int p, std::function<double(double, double)> f)
    : type(t), NameFunc(std::move(name)), prty(p), calc_func(std::move(f)) {}

Type BasicOperation::getType() const { return type; }
std::string BasicOperation::getNameFunc() const { return NameFunc; }
int BasicOperation::getPriority() const { return prty; }
double BasicOperation::calculateResult(double x, double y) const { return calc_func(x, y); }
