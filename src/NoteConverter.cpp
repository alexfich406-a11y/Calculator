#include "../include/NoteConverter.h"
#include <iterator>
#include "../include/CalculatorException.h"

NoteConverter::NoteConverter(const listOperators& lop): listOp(lop){}

bool NoteConverter::isDigigt(char c) const{
    return c >= '0' && c <= '9';
}

bool NoteConverter::isAlpha(char c) const{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool NoteConverter::is_minus_uno(const Note& note) const{
    return note.kind == KindNote::LBRACE ||
     note.kind == KindNote::OP || 
     note.kind == KindNote::COMMA;
}

Note NoteConverter::readFunc(const std::string& str, std::string::const_iterator& pos){
    auto st = pos;
    
    while(pos < str.cend() && isAlpha(*pos))
        pos++;
    
    std::string name = std::string(st, pos);
    int ind = static_cast<int>(std::distance(str.cbegin(), st));
    if(listOp.isPlugin(name))
        return Note{KindNote::FUNC, name, ind};
    
    throw LexicalException("Unknown function '" + name + "'", ind);
}


Note NoteConverter::readOperator(const std::string& str, std::string::const_iterator& pos){
    auto st = pos;
    auto res_pos = pos;
    
    while(pos < str.cend() && !isAlpha(*pos) &&
     !isDigigt(*pos) && *pos != ' ' && 
     *pos != '('  && *pos != ')'  && *pos != ',')
     {
        pos++;
        if(listOp.containsOp(std::string(st, pos)))
            res_pos = pos;
     }
    std::string name = std::string(st, res_pos);
    int ind = static_cast<int>(std::distance(str.cbegin(), st));
     pos = res_pos;
    if(st != res_pos)
        return Note{KindNote::OP, name, ind};
    
    throw LexicalException("Unknown symbol or operator", ind);
}

Note NoteConverter::readNum(const std::string& str, std::string::const_iterator& pos){
    auto st = pos;

    while (pos != str.cend() && isDigigt(*pos))
        pos++;

    if(pos <str.cend() && *pos == '.'){
        pos++;
        if(pos == str.cend() || !isDigigt(*pos))
        {
            int current_ind = static_cast<int>(std::distance(str.cbegin(), pos));
            throw LexicalException("Incorrect number: digit expected after '.'", current_ind);
        }

        while(pos < str.cend() && isDigigt(*pos))
            pos++;
    }

    std::string num = std::string(st, pos);

    if(pos <str.cend() && *pos == '.')
    {
        int current_ind = static_cast<int>(std::distance(str.cbegin(), pos));
        throw LexicalException("Incorrect number: more than one '.'", current_ind);
    }
    int ind = static_cast<int>(std::distance(str.cbegin(), st));
    return Note{KindNote::NUM, num, ind};
}

std::vector<Note> NoteConverter::expresionToNotes(const std::string& str){
    std::vector<Note> arrNotes;
    for (auto it = str.cbegin(); it < str.cend();){
        if(*it == ' '){
            it++;
            continue;
        }
        int pos_ind = static_cast<int>(std::distance(str.cbegin(), it));
        switch(*it){

            case '(':
                arrNotes.push_back(Note{KindNote::LBRACE, "(", pos_ind});
                it++;
                break;

            case ')':
                arrNotes.push_back(Note{KindNote::RBRACE, ")", pos_ind});
                it++;
                break;

            case ',':
                arrNotes.push_back(Note{KindNote::COMMA, ",", pos_ind});
                it++;
                break;

            case '-':
                if(arrNotes.empty() || is_minus_uno(arrNotes.back())){
                    arrNotes.push_back(Note{KindNote::OP, "neg", pos_ind});
                    it++;
                    break;
                }
                arrNotes.push_back(readOperator(str, it));
                break;
                

            default:
                if(isDigigt(*it))
                    arrNotes.push_back(readNum(str, it));
                else if(isAlpha(*it))
                    arrNotes.push_back(readFunc(str, it));
                else
                    arrNotes.push_back(readOperator(str, it));
                break;
        }
    }
    return arrNotes;
}
