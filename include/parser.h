#pragma once
#include <vector>
#include "listOperators.h"
#include "Note.h"

class parser {
    const listOperators& listOp;
public:
    explicit parser(const listOperators& lp);
    std::vector<Note> parserNoteToOrderedList(const std::vector<Note>& arrNotes) const;
};
