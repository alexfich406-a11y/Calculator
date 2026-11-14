#include "../include/parser.h"
#include "../include/CalculatorException.h"

#include <utility>

namespace {

bool mustPopBefore(int cur, bool curRightAssoc, int top) {
    return curRightAssoc ? cur < top : cur <= top;
}

class TransformState {
public:
    TransformState(const listOperators& ops_in, const std::vector<Note>& notes_in)
        : ops(ops_in), notes(notes_in) {}

    std::vector<Note> run() {
        for (size_t i = 0; i < notes.size(); ++i)
            handle(i);
        return finish();
    }

private:
    const listOperators& ops;
    const std::vector<Note>& notes;
    std::vector<Note> output;
    std::vector<Note> temp;
    bool waitOperand = true;   

    void handle(size_t i) {
        const Note& note = notes[i];
        switch (note.kind) {
            case KindNote::NUM:    onNumber(note);     break;
            case KindNote::FUNC:   onFunction(i);      break;
            case KindNote::LBRACE: onLeftBrace(note);  break;
            case KindNote::RBRACE: onRightBrace(note); break;
            case KindNote::COMMA:  onComma(note);      break;
            case KindNote::OP:     onOperator(note);   break;
            case KindNote::NONE:
                throw SyntaxException("Unexpected unknown note", note.pos);
        }
    }

    void requireOperandExpected(const Note& note, const char* what) const {
        if (!waitOperand)
            throw SyntaxException(std::string("Unexpected ") + what, note.pos);
    }

    void requireAfterOperand(const Note& note, const char* what) const {
        if (waitOperand)
            throw SyntaxException(std::string("Unexpected ") + what, note.pos);
    }

    void moveTopToOutput() {
        output.push_back(std::move(temp.back()));
        temp.pop_back();
    }

    bool flushToLeftBrace() {
        while (!temp.empty()) {
            if (temp.back().kind == KindNote::LBRACE)
                return true;
            moveTopToOutput();
        }
        return false;
    }

    void onNumber(const Note& note) {
        requireOperandExpected(note, "number");
        output.push_back(note);
        waitOperand = false;
    }

    void onFunction(size_t i) {
        const Note& note = notes[i];
        requireOperandExpected(note, "function");

        const bool hasNext = i + 1 < notes.size();
        if (!hasNext || notes[i + 1].kind != KindNote::LBRACE)
            throw SyntaxException("Expected '(' after function '" + note.strNote + "'",
                                  hasNext ? notes[i + 1].pos : note.pos);
        temp.push_back(note);
    }

    void onLeftBrace(const Note& note) {
        requireOperandExpected(note, "'('");
        temp.push_back(note);
    }

    void onRightBrace(const Note& note) {
        requireAfterOperand(note, "')'");
        if (!flushToLeftBrace())
            throw SyntaxException("')' without matching '('", note.pos);

        temp.pop_back();  
        if (!temp.empty() && temp.back().kind == KindNote::FUNC)
            moveTopToOutput();   
    }

    void onComma(const Note& note) {
        requireAfterOperand(note, "','");
        if (!flushToLeftBrace())
            throw SyntaxException("',' outside of function parentheses", note.pos);
        waitOperand = true;
    }

    void onOperator(const Note& note) {
        if (ops.getType(note.strNote) == Type::UNO) {
            requireOperandExpected(note, "unary operator");
            temp.push_back(note);
            return;
        }
        if (waitOperand)
            throw SyntaxException("Unexpected operator '" + note.strNote + "'", note.pos);

        const int priority = ops.getPriority(note.strNote);
        const bool rightAssoc = ops.isRightAssve(note.strNote);
        while (!temp.empty() && temp.back().kind == KindNote::OP &&
               mustPopBefore(priority, rightAssoc, ops.getPriority(temp.back().strNote)))
            moveTopToOutput();

        temp.push_back(note);
        waitOperand = true;
    }

    std::vector<Note> finish() {
        if (waitOperand && !temp.empty())
            throw SyntaxException("Incomplete expression: operand expected", notes.back().pos);

        while (!temp.empty()) {
            if (temp.back().kind == KindNote::LBRACE)
                throw SyntaxException("Unclosed '('", temp.back().pos);
            moveTopToOutput();
        }
        return std::move(output);
    }
};

}

parser::parser(const listOperators& lp) : listOp(lp) {}

std::vector<Note> parser::parserNoteToOrderedList(const std::vector<Note>& arrNotes) const {
    if (arrNotes.empty())
        return {};
    return TransformState(listOp, arrNotes).run();
}