#include "RegisterBasic.h"
#include "BasicOperation.h"
#include <memory>

void registerBasicOperations(listOperators& registry) {
    auto add = [&registry](Type type, const char* name, int prio, double (*f)(double, double)) {
        registry.addBasic(std::make_unique<BasicOperation>(type, name, prio, f));
    };

    add(Type::BIN, "+", priority::AddSub, [](double a, double b) { return a + b; });
    add(Type::BIN, "-", priority::AddSub, [](double a, double b) { return a - b; });
    add(Type::BIN, "*", priority::MulDiv, [](double a, double b) { return a * b; });
    add(Type::BIN, "/", priority::MulDiv, [](double a, double b) { return a / b; });

    // Внутреннее имя унарного минуса: пользователь его не вводит, токенайзер
    // подставляет его сам (буквенные имена из ввода ищутся только среди плагинов).
    add(Type::UNO, "neg", priority::Unary, [](double a, double) { return -a; });
}
