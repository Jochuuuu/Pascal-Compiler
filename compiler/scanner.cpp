#include <iostream>
#include <cstring>
#include "token.h"
#include "scanner.h"

using namespace std;

Scanner::Scanner(const char *s) : input(s), first(0), current(0) {}

bool is_white_space(char c)
{
    return c == ' ' || c == '\n' || c == '\r' || c == '\t';
}

// lastcomneterio = true
// a := 5; //         ddfsfd sds
Token *Scanner::nextToken()
{
    Token *token;
    if (lastcomentario != 'F')
    {

        if (current == input.size())
        {
            token = new Token(Token::ERR);
            return token;
        }
        if (lastcomentario == '/')
        {
            if (input[current] == '\n')
            {
                token = new Token(Token::ENDLINE);
                lastcomentario = 'F';
                return token;
            }
            first = current;

            while (current < input.length() && input[current] != '\n')
            {
                current++;
            }

            string word = input.substr(first, current - first);
            token = new Token(Token::ID, word, 0, word.length());
            return token;
        }

        else if (lastcomentario == '{') // {}
        {
            current++;

            if (input[current - 1] == '}')
            {
                token = new Token(Token::ENDLINE);
                lastcomentario = 'F';
                return token;
            }

            current = current - 1;

            first = current;

            while (current < input.length())
            {
                if (input[current] == '{')
                {
                    ++level;
                }
                else if (input[current] == '}')
                {
                    if (level > 1)
                    {
                        level--;
                    }
                    else
                    {
                        level--;
                        break;
                    }
                }
                current++;
            }

            string word = input.substr(first, current - first);
            token = new Token(Token::ID, word, 0, word.length());
            return token;
        }
        else if (lastcomentario == '(') // Para comentarios del tipo (* *)
        {
            current++;

            if (input[current - 1] == '*' && current < input.length() && input[current] == ')')
            {
                token = new Token(Token::ENDLINE);
                lastcomentario = 'F';
                current += 1;
                return token;
            }
            current = current - 1;

            first = current;
            while (current < input.length() )
            {

                 if ((input[current] == '(' && current + 1 < input.length() && input[current + 1] == '*')   )
                {
                    current++;
                    ++level;
                }
                else if ((input[current] == '*' && current + 1 < input.length() && input[current + 1] == ')') )
                {
                    if (level > 1)
                    {
                        level--;
                    }
                    else
                    {
                        level--;
                        break;
                    }
                }

            
                current++;
            }

            string word = input.substr(first, current - first);
            token = new Token(Token::ID, word, 0, word.length());

            return token;
        }
    }

    while (current < input.length() && is_white_space(input[current]))
        current++;
    if (current >= input.length())
        return new Token(Token::END);
    char c = input[current];
    first = current;
    // Detectar números
    if (isdigit(c))
    {
        current++;
        while (current < input.length() && isdigit(input[current]))
            current++;
        token = new Token(Token::NUM, input, first, current - first);
    }

    // Detectar palabras clave e identificadores
    else if (isalpha(c))
    {
        current++;
        while (current < input.length() && isalnum(input[current]))
            current++;
        string word = input.substr(first, current - first);
        if (word == "procedure")
        {
            token = new Token(Token::FUN, word, 0, word.length());
        }

        else if (word == "function")
        {
            token = new Token(Token::FUN, word, 0, word.length());
        }
         else if (word == "uses")
        {
            token = new Token(Token::USES, word, 0, word.length());
        }
        else if (word == "if")
        {
            token = new Token(Token::IF, word, 0, word.length());
        }
        else if (word == "to")
        {
            token = new Token(Token::TO, word, 0, word.length());
        }
        else if (word == "then")
        {
            token = new Token(Token::THEN, word, 0, word.length());
        }
        else if (word == "else")
        {
            token = new Token(Token::ELSE, word, 0, word.length());
        }

         else if (word == "downto")
        {
            token = new Token(Token::DOWNTO, word, 0, word.length());
        }

        else if (word == "for")
        {
            token = new Token(Token::FOR, word, 0, word.length());
        }
        else if (word == "begin")
        {
            token = new Token(Token::BEGIN, word, 0, word.length());
        }

        else if (word == "end")
        {
            token = new Token(Token::END_BLOCK, word, 0, word.length()); // Detecta end
        }
        else if (word == "var")
        {
            token = new Token(Token::VAR, word, 0, word.length());
        }
        else if (word == "integer")
        {
            token = new Token(Token::ID, word, 0, word.length()); // integer se maneja como un identificador de tipo
        }
        else if (word == "writeln")
        {
            token = new Token(Token::PRINT, word, 0, word.length());
        }
        else if (word == "return")
        {
            token = new Token(Token::RETURN, word, 0, word.length());
        }
        else if (word == "while")
        {
            token = new Token(Token::WHILE, word, 0, word.length());
        }
        else if (word == "ifexp")
        {
            token = new Token(Token::IFEXP, word, 0, word.length());
        }
        else if (word == "do")
        {
            token = new Token(Token::DO, word, 0, word.length());
        }
        else if (word == "Program" || word == "program")
        {
            token = new Token(Token::PROGRAM, word, 0, word.length());
        }
        else
        {
            token = new Token(Token::ID, word, 0, word.length());
        }
    }
    // Detectar operadores y símbolos
    else if (strchr("+-*/():;,.<{", c))
    {
        switch (c)
        {
        case '+':
            token = new Token(Token::PLUS, c);
            break;
        case '.':
            token = new Token(Token::DOT, c);
            break;
        case '-':
            token = new Token(Token::MINUS, c);
            break;
        case '*':

            token = new Token(Token::MUL, c);
            break;
        case '/':
            if (current + 1 < input.length() && input[current + 1] == '/')
            {
                token = new Token(Token::COMMENT, "//", 0, 2);
                current++;
                lastcomentario = '/';
                level = 1;
            }
            else
            {
                token = new Token(Token::DIV, c);
            }
            break;

        case ',':
            token = new Token(Token::COMA, c);
            break;
        case '{':
            token = new Token(Token::COMMENT, c);
            lastcomentario = '{';
            level = 1;

            break;
        case '(':
            if (current + 1 < input.length() && input[current + 1] == '*')
            {
                token = new Token(Token::COMMENT, "(*", 0, 2);
                current++;
                lastcomentario = '(';
                level = 1;
            }
            else
            {
                token = new Token(Token::PI, c);
            }
            break;
        case ')':
            token = new Token(Token::PD, c);
            break;
        case ':':
            if (current + 1 < input.length() && input[current + 1] == '=')
            {
                token = new Token(Token::ASSIGN, ":=", 0, 2);
                current++;
            }
            else
            {
                token = new Token(Token::COLON, c);
            }
            break;

        case '<':
            if (current + 1 < input.length() && input[current + 1] == '=')
            {
                token = new Token(Token::LE, "<=", 0, 2);
                current++;
            }
            else
            {
                token = new Token(Token::LT, c);
            }
            break;
        case ';':
            token = new Token(Token::PC, c);
            break;
        default:
            cout << "Error: símbolo no reconocido " << c << endl;
            token = new Token(Token::ERR, c);
        }
        current++;
    }
    else
    {
        token = new Token(Token::ERR, c);
        current++;
    }
    return token;
}

void Scanner::reset()
{
    first = 0;
    current = 0;
}

Scanner::~Scanner() {}

void test_scanner(Scanner *scanner)
{
    Token *current;
    cout << "Iniciando Scanner:" << endl
         << endl;
    while ((current = scanner->nextToken())->type != Token::END)
    {
        if (current->type == Token::ERR)
        {
            cout << "Error en scanner - carácter inválido: " << current->text << endl;
            break;
        }
        else
        {
            cout << *current << endl;
        }
        delete current;
    }
    cout << "TOKEN(END)" << endl;
    delete current;
}
