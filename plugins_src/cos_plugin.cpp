#include "../include/plugin_helpers.h"
#include <cmath>

namespace {
class CosPlugin final : public FunctionPlugin {
public:
    CosPlugin()
        : FunctionPlugin(Type::UNO, "cos", priority::Function,
                         [](double x, double /*unused*/) { return std::cos(x * 3.14159265359 / 180.0); }) {}
};
}

PLUGIN_DEFINE(CosPlugin)
