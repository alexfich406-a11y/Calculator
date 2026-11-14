#include "../include/CalcTree.h"
#include <stdexcept>
#include <string>
#include "../include/CalculatorException.h"

CalcTree::CalcTree(const listOperators& lp) : listOp(lp) {}

NodePtr CalcTree::createTree(const std::vector<Note>& arrOrdNotes) const {
    if (arrOrdNotes.empty()) return nullptr;

    std::vector<NodePtr> temp;

    for (const auto& note : arrOrdNotes) {
        if (note.kind == KindNote::NUM) {
            double val = 0.0;
            try {
                val = std::stod(note.strNote); 
            } catch (const std::out_of_range&) {
                throw LexicalException("Number is too large", note.pos);
            } catch (const std::invalid_argument&) {
                throw LexicalException("Incorrect number", note.pos);
            }
            temp.push_back(std::make_unique<NumNode>(val));
            continue;
        }

        const PluginInterface* op = listOp.findOperation(note.strNote);

        if (op->getType() == Type::UNO) {
            if (temp.empty())
                throw SyntaxException("Operand expected for '" + note.strNote + "'", note.pos);
            auto operand = std::move(temp.back());
            temp.pop_back();
            temp.push_back(std::make_unique<OpNode>(op, std::move(operand)));
        } else {
            if (temp.size() < 2)
                throw SyntaxException("Two operands expected for '" + note.strNote + "'", note.pos);
            auto rightNode = std::move(temp.back());
            temp.pop_back();
            auto leftNode = std::move(temp.back());
            temp.pop_back();
            temp.push_back(std::make_unique<OpNode>(op, std::move(leftNode), std::move(rightNode)));
        }
    }

    if (temp.size() != 1)
        throw SyntaxException("Wrong number of arguments (extra operands left)", arrOrdNotes.back().pos);

    return std::move(temp.back());
}
