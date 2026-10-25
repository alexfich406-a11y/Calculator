#include <iomanip>
#include <iostream>
#include <string>

#include "BasicOperation/RegisterBasic.h"
#include "../include/CalcTree.h"
#include "../include/CalculatorException.h"
#include "../include/ExecutablePath.h"
#include "../include/NoteConverter.h"
#include "../include/PluginLoader.h"
#include "../include/listOperators.h"
#include "../include/parser.h"

namespace {


std::string trim(const std::string& s) {
    auto b = s.find_first_not_of(" \t\r\n");
    if (b == std::string::npos) return "";
    auto e = s.find_last_not_of(" \t\r\n");
    return s.substr(b, e - b + 1);
}

double evaluate(const std::string& line, const listOperators& registry, bool& hasValue) {
    NoteConverter tokenizer(registry);
    parser syntax(registry);
    CalcTree builder(registry);

    auto notes = tokenizer.expresionToNotes(line);
    auto rpn = syntax.parserNoteToOrderedList(notes);
    NodePtr root = builder.createTree(rpn);
    hasValue = (root != nullptr);
    return root ? root->calculate() : 0.0;
}

} 

int main(int argc, char** argv) {
    (void)argc;

    listOperators registry;
    registerBasicOperations(registry);
    PluginLoader loader(registry, executableDir(argv[0]) / "plugins");

    std::cout << "Calculator. Type an expression, or 'exit' to quit.\n";
    std::cout << std::setprecision(15);

    for (const auto& message : loader.refresh())
        std::cout << message << '\n';
    if (loader.loadedCount() == 0)
        std::cout << "[plugins] no plugins loaded (folder: " << loader.directory().string()
                  << "); only + - * / and () are available\n";

    std::string line;
    while (true) {
        std::cout << "> " << std::flush;
        if (!std::getline(std::cin, line)) break;

        const std::string command = trim(line);
        if (command.empty()) continue;
        if (command == "exit" || command == "quit") break;

        for (const auto& message : loader.refresh())
            std::cout << message << '\n';

        try {
            bool hasValue = false;
            double value = evaluate(line, registry, hasValue);
            if (hasValue) std::cout << value << '\n';
        }
        catch (const PositionedException& e) {    
            std::cout << std::string(2 + e.getPosition(), ' ') << "^\n";
            std::cout << e.what() << '\n';
        }
        catch (const CalculatorException& e) {     
            std::cout << e.what() << '\n';
        }
        catch (const std::exception& e) {      
            std::cout << "[Internal Error] " << e.what() << '\n';
        }
        catch (...) { 
            std::cout << "[Internal Error] unknown exception\n";
        }
    }
    return 0;
}
