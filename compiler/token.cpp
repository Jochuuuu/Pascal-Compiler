#include <iostream>
#include "token.h"

using namespace std;

Token::Token(Type type):type(type) { text = ""; }

Token::Token(Type type, char c):type(type) { text = string(1, c); }

Token::Token(Type type, const string& source, int first, int last):type(type) {
    text = source.substr(first, last);
}

std::ostream& operator<<(std::ostream& outs, const Token& tok)
{
    switch (tok.type) {
        case Token::PLUS: outs << "TOKEN(PLUS)"; break;
        case Token::MINUS: outs << "TOKEN(MINUS)"; break;
        case Token::MUL: outs << "TOKEN(MUL)"; break;
        case Token::DIV: outs << "TOKEN(DIV)"; break;
        case Token::NUM: outs << "TOKEN(NUM)"; break;
        case Token::ERR: outs << "TOKEN(ERR)"; break;
        case Token::PD: outs << "TOKEN(PD)"; break;
        case Token::PI: outs << "TOKEN(PI)"; break;
        case Token::END: outs << "TOKEN(END)"; break;
        case Token::ID: outs << "TOKEN(ID)"; break;
        case Token::PRINT: outs << "TOKEN(PRINT)"; break; // Reusamos PRINT para WRITELN
        case Token::ASSIGN: outs << "TOKEN(ASSIGN)"; break;
        case Token::PC: outs << "TOKEN(PC)"; break;
        case Token::LT: outs << "TOKEN(LT)"; break;
        case Token::LE: outs << "TOKEN(LE)"; break;
        case Token::EQ: outs << "TOKEN(EQ)"; break;
        case Token::IF: outs << "TOKEN(IF)"; break;
                case Token::IFEXP: outs << "TOKEN(IFEXP)"; break;

        case Token::THEN: outs << "TOKEN(THEN)"; break;
        case Token::ELSE: outs << "TOKEN(ELSE)"; break;
        case Token::ENDIF: outs << "TOKEN(ENDIF)"; break;
        case Token::WHILE: outs << "TOKEN(WHILE)"; break;
        case Token::DO: outs << "TOKEN(DO)"; break;
        case Token::ENDWHILE: outs << "TOKEN(ENDWHILE)"; break;
        case Token::COMA: outs << "TOKEN(COMA)"; break;
        case Token::VAR: outs << "TOKEN(VAR)"; break;
        case Token::FOR: outs << "TOKEN(FOR)"; break;
        case Token::ENDFOR: outs << "TOKEN(ENDFOR)"; break;
        case Token::TRUE: outs << "TOKEN(TRUE)"; break;
        case Token::FALSE: outs << "TOKEN(FALSE)"; break;
        case Token::RETURN: outs << "TOKEN(RETURN)"; break;
        case Token::FUN: outs << "TOKEN(FUN)"; break;
        case Token::ENDFUN: outs << "TOKEN(ENDFUN)"; break;
        case Token::BEGIN: outs << "TOKEN(BEGIN)"; break; // Nuevo token para begin
        case Token::END_BLOCK: outs << "TOKEN(END_BLOCK)"; break;   // Nuevo token para end
        case Token::COLON: outs << "TOKEN(COLON)"; break;   // Nuevo token para end
        case Token::PROGRAM: outs << "TOKEN(PROGRAM)"; break;   // Nuevo token para end
        case Token::DOT: outs << "TOKEN(DOT)"; break;   // Nuevo token para end

        //case Token::WRITELN: outs << "TOKEN(WRITELN)"; break; // WRITELN
        default: outs << "TOKEN(UNKNOWN)"; break;
    }
    return outs;
}

std::ostream& operator<<(std::ostream& outs, const Token* tok) {
    return outs << *tok;
}