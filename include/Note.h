#pragma once
#include <string>
enum class KindNote { FUNC, NUM, OP, LBRACE, RBRACE, COMMA, NONE };
struct Note {
    KindNote kind {KindNote::NONE};
    std::string strNote;
    int pos{0};
};
