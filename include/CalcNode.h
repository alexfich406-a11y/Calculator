#pragma once
#include <cmath>
#include <memory>
#include <utility>
#include "CalculatorException.h"
#include "plugin.h"

class Node {
public:
    virtual ~Node() = default;
    virtual double calculate() const = 0;
};

using NodePtr = std::unique_ptr<Node>;

class NumNode final : public Node {
    double value;
public:
    explicit NumNode(double val) : value(val) {}
    double calculate() const override { return value; }
};

class OpNode final : public Node {
    const PluginInterface* operation;
    NodePtr left;    
    NodePtr right;  

public:
    OpNode(const PluginInterface* op, NodePtr l, NodePtr r)
        : operation(op), left(std::move(l)), right(std::move(r)) {}

    OpNode(const PluginInterface* op, NodePtr operand)
        : operation(op), left(nullptr), right(std::move(operand)) {}

    double calculate() const override {
        double result;
        if (left) {
            double a = left->calculate();  
            double b = right->calculate();
            result = operation->calculateResult(a, b);
        } else {
            result = operation->calculateResult(right->calculate(), 0.0);
        }

        if (std::isnan(result))
            throw MathException("Operation '" + operation->getNameFunc() + "' resulted in an undefined value (NaN)");
        if (std::isinf(result))
            throw MathException("Operation '" + operation->getNameFunc() + "' caused overflow or division by zero (infinity)");
        return result;
    }
};
