#include "../include/plugin_helpers.h"
#include <cmath>

namespace {
class LnPlugin final : public FunctionPlugin {
public:
    LnPlugin()
        : FunctionPlugin(Type::UNO, "ln", priority::Function,
                         [](double x, double /*unused*/) { return std::log(x); }) {}
};
}

PLUGIN_DEFINE(LnPlugin)
