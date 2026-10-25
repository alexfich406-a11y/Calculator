#pragma once
#include <stdexcept>
#include <string>

class CalculatorException : public std::runtime_error {
public:
    explicit CalculatorException(const std::string& message)
        : std::runtime_error(message) {}
    virtual ~CalculatorException() = default;
};

class PositionedException : public CalculatorException {
private:
    size_t position;
public:
    PositionedException(const std::string& message, size_t pos)
        : CalculatorException(message), position(pos) {}
    size_t getPosition() const  { return position; }
};

class LexicalException final : public PositionedException {
public:
    LexicalException(const std::string& message, size_t pos)
        : PositionedException("[Lexical Error] " + message, pos) {}
};

class SyntaxException final : public PositionedException {
public:
    SyntaxException(const std::string& message, size_t pos)
        : PositionedException("[Syntax Error] " + message, pos) {}
};

class MathException final : public CalculatorException {
public:
    explicit MathException(const std::string& message)
        : CalculatorException("[Math Error] " + message) {}
};

class PluginException final : public CalculatorException {
public:
    explicit PluginException(const std::string& message)
        : CalculatorException("[Plugin Error] " + message) {}
};
