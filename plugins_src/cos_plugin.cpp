#include "../include/plugin_helpers.h"
#include <cmath>

namespace {
class CosPlugin final : public FunctionPlugin {
public:
    CosPlugin()
        : FunctionPlugin(Type::UNO, "cos", priority::Function,
                         [](double x, double /*unused*/) { return std::cos(x * (M_PI / 180.0)); }) {}
};
}

PLUGIN_DEFINE(CosPlugin)
