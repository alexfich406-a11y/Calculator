#pragma once
#include <string>
#include <vector>
#include "Note.h"
#include "listOperators.h"

class NoteConverter {
    const listOperators& listOp;

    Note readFunc(const std::string& str, std::string::const_iterator& pos);
    Note readOperator(const std::string& str, std::string::const_iterator& pos);
    Note readNum(const std::string& str, std::string::const_iterator& pos);

    bool isAlpha(char ch) const;
    bool isDigigt(char ch) const;
    bool is_minus_uno(const Note& note) const;

public:
    explicit NoteConverter(const listOperators& lop);
    std::vector<Note> expresionToNotes(const std::string& str);
};
