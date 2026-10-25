#pragma once
#include <string>
enum class KindNote { FUNC, NUM, OP, LBRACE, RBRACE, COMMA };
struct Note {
    KindNote kind;
    std::string strNote;
    int pos;
};
