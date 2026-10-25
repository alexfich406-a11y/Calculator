#include "../include/plugin_helpers.h"
#include <cmath>

namespace {
class AbsPlugin final : public FunctionPlugin {
public:
    AbsPlugin()
        : FunctionPlugin(Type::UNO, "abs", priority::Function,
                         [](double x, double /*unused*/) { return std::fabs(x); }) {}
};
}

PLUGIN_DEFINE(AbsPlugin)
