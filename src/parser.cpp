#include "../include/parser.h"
#include "../include/CalculatorException.h"

parser::parser(const listOperators& lp): listOp(lp){}

std::vector<Note> parser::parserNoteToOrderedList(const std::vector<Note>& arrNotes) const{
    if (arrNotes.empty())
        return {};
    
    std::vector<Note> output;
    std::vector<Note> temp;

    bool waitOperand = true; //flag for work with under code
    for(size_t i = 0; i < arrNotes.size(); ++i) {
        const auto& note = arrNotes[i];


        switch (note.kind)
        {
            case KindNote::NUM:{
                if(!waitOperand)
                    throw SyntaxException("Unexpected number", note.pos);
                output.push_back(note);
                waitOperand = false;
                break;
            }
            case KindNote::FUNC:{
                if(!waitOperand)
                    throw SyntaxException("Unexpected function", note.pos);
                if(i + 1 >= arrNotes.size() || arrNotes[i + 1].kind != KindNote::LBRACE)
                    throw SyntaxException("Expected '(' after function '" + note.strNote + "'",
                                          i + 1 < arrNotes.size() ? arrNotes[i + 1].pos : note.pos);

                temp.push_back(note);
                break;
            }
            case KindNote::LBRACE:{
                if(!waitOperand)
                    throw SyntaxException("Unexpected '('", note.pos);

                temp.push_back(note);
                break;
            }
            case KindNote::RBRACE:{
                if(waitOperand)
                    throw SyntaxException("Unexpected ')'", note.pos);
                
                bool isExistLbrace = false;
                while (!temp.empty()) {
                    if(temp.back().kind == KindNote::LBRACE){
                        isExistLbrace = true;
                        temp.pop_back();
                        break;
                    }
                    output.push_back(temp.back());
                    temp.pop_back();
                }
                
                if (!isExistLbrace) {
                    throw SyntaxException("')' without matching '('", note.pos);
                }

                if(!temp.empty() && temp.back().kind == KindNote::FUNC )
                {
                    output.push_back(temp.back());
                    temp.pop_back();
                }

                break;
            }

            case KindNote::COMMA:{
                if(waitOperand)
                    throw SyntaxException("Unexpected ','", note.pos);
                            bool isExistLbrace = false;
                
                            while (!temp.empty()) {
                    if(temp.back().kind == KindNote::LBRACE){
                        isExistLbrace = true;
                        break;
                    }
                    output.push_back(temp.back());
                    temp.pop_back();
                }

                if(!isExistLbrace)
                    throw SyntaxException("',' outside of function parentheses", note.pos);
                
                waitOperand = true;
                break;

            }
            case KindNote::OP:{
                if(listOp.getType(note.strNote) == Type::UNO)
                {
                    if(!waitOperand)
                        throw SyntaxException("Unexpected unary operator", note.pos);
                    temp.push_back(note);
                    break;
                }

                if(waitOperand)
                    throw SyntaxException("Unexpected operator '" + note.strNote + "'", note.pos);

                int curPrty = listOp.getPriority(note.strNote);
                bool curRightAssve = listOp.isRightAssve(note.strNote);

                while (!temp.empty() &&  temp.back().kind == KindNote::OP)
                {
                    int topPrty = listOp.getPriority(temp.back().strNote);
                    if ((!curRightAssve && (curPrty <= topPrty)) || (curRightAssve && (curPrty < topPrty)))
                    {
                        output.push_back(temp.back());
                        temp.pop_back();
                    }
                    else
                        break;
                }
                temp.push_back(note);
                waitOperand = true;
                break;
            }
        }
    }

    if(waitOperand && !temp.empty() )
            throw SyntaxException("Incomplete expression: operand expected", arrNotes.back().pos) ;
    
    while(!temp.empty())
    {
        if(temp.back().kind == KindNote::LBRACE)
            throw SyntaxException("Unclosed '('", temp.back().pos) ;

        output.push_back(temp.back());
        temp.pop_back();
    }
    
    return output;
}
