#pragma once
#include <vector>
#include "CalcNode.h"
#include "Note.h"
#include "listOperators.h"

class CalcTree final {
    const listOperators& listOp;

public:
    explicit CalcTree(const listOperators& lp);
    NodePtr createTree(const std::vector<Note>& arrOrdNotes) const;
};
