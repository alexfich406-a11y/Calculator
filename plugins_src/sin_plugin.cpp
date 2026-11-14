#include "../include/plugin_helpers.h"
#include <cmath>

namespace {
class SinPlugin final : public FunctionPlugin {
public:
    SinPlugin()
        : FunctionPlugin(Type::UNO, "sin", priority::Function,
                         [](double x, double ) { return std::sin(x * 3.14159265359 / 180.0); }) {}
};
}

PLUGIN_DEFINE(SinPlugin)
