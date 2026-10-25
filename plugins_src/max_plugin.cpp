#include "../include/plugin_helpers.h"

namespace {
class MaxPlugin final : public FunctionPlugin {
public:
    MaxPlugin()
        : FunctionPlugin(Type::BIN, "max", priority::Function,
                         [](double x, double y) { return x > y ? x : y; }) {}
};
}

PLUGIN_DEFINE(MaxPlugin)
