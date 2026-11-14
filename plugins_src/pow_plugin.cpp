#include "../include/plugin_helpers.h"
#include <cmath>

namespace {
class PowPlugin final : public FunctionPlugin {
public:
    PowPlugin()
        : FunctionPlugin(Type::BIN, "^", priority::Power,
                         [](double x, double y) { return std::pow(x, y); },
                         /*rightAssoc=*/true) {}
};
}

PLUGIN_DEFINE(PowPlugin)
