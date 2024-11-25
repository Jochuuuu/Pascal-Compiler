#include <iostream>
#include <stdexcept>
#include "token.h"
#include "scanner.h"
#include "exp.h"
#include "parser.h"

using namespace std;

bool Parser::match(Token::Type ttype)
{
    if (check(ttype))
    {
        advance();
        return true;
    }
    return false;
}

bool Parser::check(Token::Type ttype)
{
    if (isAtEnd())
        return false;
    return current->type == ttype;
}

bool Parser::advance()
{
    cout << "Token actual: " << *current << endl;

    if (!isAtEnd())
    {
        Token *temp = current;
        if (previous)
            delete previous;
        current = scanner->nextToken();
        previous = temp;
        if (check(Token::ERR))
        {
            cout << "Error de análisis, carácter no reconocido: " << current->text << endl;
            exit(1);
        }
        return true;
    }
    return false;
}

bool Parser::isAtEnd()
{
    return (current->type == Token::END);
}

Parser::Parser(Scanner *sc) : scanner(sc)
{
    previous = NULL;
    current = scanner->nextToken();
    if (current->type == Token::ERR)
    {
        cout << "Error en el primer token: " << current->text << endl;
        exit(1);
    }
}
VarDec *Parser::parseVarDec()
{
    VarDec *vd = NULL;
    list<string> ids;
    string type;

    // Procesar nombres de variables separados por comas
    if (!match(Token::ID))
    {
        cout << "Error: se esperaba un identificador después de 'var'." << endl;
        exit(1);
    }
    ids.push_back(previous->text);
    while (match(Token::COMA))
    { // Procesar múltiples nombres separados por ','
        if (!match(Token::ID))
        {
            cout << "Error: se esperaba un identificador después de ','." << endl;
            exit(1);
        }
        ids.push_back(previous->text);
    }

    // Verificar y procesar ':'
    if (!match(Token::COLON))
    {
        cout << "Error: se esperaba ':' después de los identificadores." << endl;
        exit(1);
    }

    // Leer el tipo de las variables
    if (!match(Token::ID) || previous->text != "integer")
    {
        cout << "Error: solo se permite el tipo 'integer'." << endl;
        exit(1);
    }
    type = "integer";

    vd = new VarDec(type, ids); // Crear la declaración de variables
    return vd;
}

VarDecList *Parser::parseVarDecList()
{
    VarDecList *vdl = new VarDecList();
    while (check(Token::VAR) || check(Token::ID))
    {  
        if (check(Token::VAR))
        {
            advance();  
        }

        VarDec *vd = parseVarDec();  
        if (vd != NULL)
        {
            vdl->add(vd);
        }

        if (!match(Token::PC))
        {
            cout << "Error: se esperaba ';' después de una declaración de variables." << endl;
            exit(1);
        }
    }
    return vdl;
}

StatementList *Parser::parseStatementList()
{
    StatementList *sl = new StatementList();

    while (!check(Token::END_BLOCK))
    { // Procesar hasta encontrar 'end'
        if (check(Token::PC))
        { // Ignorar ';' solitario
            advance();
            continue;
        }
        

        // Parsear una sentencia válida
        Stm *statement = parseStatement();
        if (statement != NULL)
        {
            sl->add(statement);
        }

        // Si el próximo token es ';', lo avanzamos
        if (check(Token::PC))
        {
            advance();
        }
    }

    return sl;
}

Body *Parser::parseBody()
{
    VarDecList *vdl = parseVarDecList(); // Procesar las declaraciones de variables
    cout << "Token después de var: " << *current << endl;

    if (!match(Token::BEGIN))
    {
        cout << "Error: se esperaba 'begin' después de las declaraciones de variables." << endl;
        exit(1);
    }
    StatementList *sl = parseStatementList(); // Procesar la lista de sentencias
    if (!match(Token::END_BLOCK))
    {
        cout << "Error: se esperaba 'end' al final del bloque." << endl;
        exit(1);
    }
    return new Body(vdl, sl);
}

FunDec *Parser::parseFunDec()
{
    FunDec *fd = NULL;

    if (match(Token::FUN))
    { // Manejar 'procedure' o 'function'
        Body *body = NULL;
        string rtype;

        // Detectar si es un procedimiento o una función
        if (previous->text == "procedure")
        {
            rtype = "void"; // Procedimientos no tienen tipo de retorno
        }
        else if (previous->text == "function")
        {
            rtype = "";  
        }
        else
        {
            cout << "Error: se esperaba 'procedure' o 'function', se encontró: " << previous->text << endl;
            exit(1);
        }

        // Leer el nombre del procedimiento o función
        if (!match(Token::ID))
        {
            cout << "Error: se esperaba un nombre después de 'procedure' o 'function'." << endl;
            exit(1);
        }
        string fname = previous->text;
        currentFunctionName = fname;

        // Leer la lista de parámetros
        list<string> types;
        list<string> vars;
        if (!match(Token::PI))
        {
            cout << "Error: se esperaba '(' después del nombre." << endl;
            exit(1);
        }
        if (!check(Token::PD))
        { // Si no es ')', hay parámetros
            if (!match(Token::ID))
            { // Nombre del parámetro
                cout << "Error: se esperaba un nombre en los parámetros." << endl;
                exit(1);
            }
            vars.push_back(previous->text); // Guardar el nombre
            if (!match(Token::COLON))
            { // Verificar ':'
                cout << "Error: se esperaba ':' después del nombre del parámetro." << endl;
                exit(1);
            }
            if (!match(Token::ID) || previous->text != "integer")
            { // Tipo del parámetro
                cout << "Error: solo se permite el tipo 'integer' en los parámetros." << endl;
                exit(1);
            }
            types.push_back("integer");
            while (match(Token::COMA))
            { // Procesar parámetros adicionales
                if (!match(Token::ID))
                {
                    cout << "Error: se esperaba un nombre en los parámetros." << endl;
                    exit(1);
                }
                vars.push_back(previous->text);
                if (!match(Token::COLON))
                {
                    cout << "Error: se esperaba ':' después del nombre del parámetro." << endl;
                    exit(1);
                }
                if (!match(Token::ID) || previous->text != "integer")
                {
                    cout << "Error: solo se permite el tipo 'integer' en los parámetros." << endl;
                    exit(1);
                }
                types.push_back("integer");
            }
        }
        if (!match(Token::PD))
        {
            cout << "Error: se esperaba ')' después de los parámetros." << endl;
            exit(1);
        }

        // Leer el tipo de retorno para funciones
        if (rtype.empty())
        { // Si es una función
            if (!match(Token::COLON))
            {
                cout << "Error: se esperaba ':' después de los parámetros en la función." << endl;
                exit(1);
            }
            if (!match(Token::ID) || previous->text != "integer")
            {
                cout << "Error: el tipo de retorno debe ser 'integer'." << endl;
                exit(1);
            }
            rtype = "int";
        }

        // Verificar el ';' después de los parámetros o tipo de retorno
        if (!match(Token::PC))
        {
            cout << "Error: se esperaba ';' después de los parámetros o tipo de retorno." << endl;
            exit(1);
        }

        // Procesar el cuerpo del procedimiento o función
        body = parseBody();
        // Verificar que el bloque termine con 'end;'

        if (!match(Token::PC))
        {
            cout << "Error: se esperaba ';' después de 'end'." << endl;
            exit(1);
        }

        fd = new FunDec(fname, types, vars, rtype, body);
    }
    return fd;
}

FunDecList *Parser::parseFunDecList()
{
    FunDecList *fdl = new FunDecList();
    FunDec *aux;
    aux = parseFunDec();
    while (aux != NULL)
    {
        fdl->add(aux);
        aux = parseFunDec();
    }
    return fdl;
}

Program *Parser::parseProgram()
{

    string nombre_programa = "";

    if (match(Token::PROGRAM))
    {
        if (!match(Token::ID))
        {
            cout << "Error: se esperaba un identificador después de 'PROGRAM'." << endl;
            exit(1);
        }

        nombre_programa = previous->text;

        if (!match(Token::PC))
        {
            cout << "Error: se esperaba ';' después del identificador de 'PROGRAM'." << endl;
            exit(1);
        }
    }
    else
    {
        cout << "Error: se esperaba 'Program' para iniciar el parser" << endl;
        exit(1);
    }

    VarDecList *v = parseVarDecList();
    FunDecList *b = parseFunDecList();

     if (match(Token::BEGIN)) {
        StatementList* sl = parseStatementList(); 
        if (!match(Token::END_BLOCK)) {
            cout << "Error: se esperaba 'END' al final del bloque principal." << endl;
            exit(1);
        }
 if (!match(Token::DOT)) {
            cout << "Error: se esperaba '.' despues de 'END'." << endl;
            exit(1);
        }
        // Crear un procedimiento 'main' automáticamente
        Body* mainBody = new Body(new VarDecList(), sl);
        FunDec* mainFunction = new FunDec("main", list<string>(), list<string>(), "void", mainBody);
        b->add(mainFunction);
    }

    
    return new Program(v, b, nombre_programa);
}

list<Stm *> Parser::parseStmList()
{
    list<Stm *> slist;
    slist.push_back(parseStatement());
    while (match(Token::PC))
    {
        slist.push_back(parseStatement());
    }
    return slist;
}

Stm *Parser::parseStatement()
{
    Stm *s = NULL;
    Exp *e = NULL;
    Body *tb = NULL; // true case
    Body *fb = NULL; // false case

    if (current == NULL)
    {
        cout << "Error: Token actual es NULL" << endl;
        exit(1);
    }

    if (check(Token::VAR))
    {
        cout << "Error: 'var' no es válido dentro de un bloque de sentencias. Las declaraciones de variables deben estar antes de 'begin'." << endl;
        exit(1);
    }

    if (match(Token::ID))
    {
        string lex = previous->text;

        if (match(Token::ASSIGN))
        {
            e = parseCExp();

            // Si el identificador coincide con el nombre de la función actual
            if (!currentFunctionName.empty() && lex == currentFunctionName)
            {
                // Interpretar como un "return implícito"
                s = new ReturnStatement(e);
            }
            else
            {
                // Asignación normal
                s = new AssignStatement(lex, e);
            }
        }
        else
        {
            if (match(Token::PI))
            {
                list<Exp *> args;
                if (!check(Token::PD))
                {
                    args.push_back(parseCExp());
                    while (match(Token::COMA))
                    {
                        args.push_back(parseCExp());
                    }
                }
                if (!match(Token::PD))
                {
                    cout << "Falta paréntesis derecho" << endl;
                    exit(0);
                }
                s = new FCallStatement(lex, args);
            }
        }
    }
    else if (match(Token::PRINT))
    {
        if (!match(Token::PI))
        {
            cout << "Error: se esperaba un '(' después de 'print'." << endl;
            exit(1);
        }
        e = parseCExp();
        if (!match(Token::PD))
        {
            cout << "Error: se esperaba un ')' después de la expresión." << endl;
            exit(1);
        }
        s = new PrintStatement(e);
    }

    else if (check(Token::PC))
    {
        // Sentencia vacía, retorna NULL
        return NULL;
    }
    else if (match(Token::IF))
    {
        // Parsear la condición del `if`
        e = parseCExp();

        // Verificar que venga 'then'
        if (!match(Token::THEN))
        {
            cout << "Error: se esperaba 'then' después de la condición." << endl;
            exit(1);
        }

        // Parsear el bloque verdadero (tb)
        tb = parseBody();
        if (match(Token::PC))
        {

            if (match(Token::ELSE))
            {
                cout << "Error: no se permite ';' antes de 'else'." << endl;
                exit(1);
            }
        }

        cout << previous->text << endl;
        // Verificar si hay un 'else'
        if (match(Token::ELSE))
        {
            // Parsear el bloque falso (fb)
            fb = parseBody();
        }
        else
        {

            if (!(previous->type == Token::PC))
            {
                cout << "Error: se esperaba ';' después de la expresión." << endl;
                exit(1);
            }
        }

        s = new IfStatement(e, tb, fb);
    }

    else if (match(Token::WHILE))
    {
        // Parsear la condición del bucle
        e = parseCExp();

        // Verificar que después de la condición venga 'do'
        if (!match(Token::DO))
        {
            cout << "Error: se esperaba 'do' después de la expresión." << endl;
            exit(1);
        }

        // Procesar el cuerpo del while hasta encontrar 'end'
        tb = parseBody();

        // Crear la declaración del while
        s = new WhileStatement(e, tb);
    }

    else if (match(Token::FOR))
    {
        if (!match(Token::PI))
        {
            cout << "Error: se esperaba '(' después de 'for'." << endl;
            exit(1);
        }
        Exp *start = parseCExp();
        if (!match(Token::COMA))
        {
            cout << "Error: se esperaba ',' después de la expresión." << endl;
            exit(1);
        }
        Exp *end = parseCExp();
        if (!match(Token::COMA))
        {
            cout << "Error: se esperaba ',' después de la expresión." << endl;
            exit(1);
        }
        Exp *step = parseCExp();
        if (!match(Token::PD))
        {
            cout << "Error: se esperaba ')' después de la expresión." << endl;
            exit(1);
        }
        tb = parseBody();
        if (!match(Token::ENDFOR))
        {
            cout << "Error: se esperaba 'endfor' al final de la declaración." << endl;
            exit(1);
        }
        s = new ForStatement(start, end, step, tb);
    }
    else if (match(Token::RETURN))
    {
        if (!match(Token::PI))
        {
            cout << "Error: se esperaba '(' después de 'return'." << endl;
            exit(1);
        }
        if (!check(Token::PD))
        {
            e = parseCExp();
        }
        if (!match(Token::PD))
        {
            cout << "Error: se esperaba ')' después de la expresión." << endl;
            exit(1);
        }
        s = new ReturnStatement(e); // Si es null, no hay problema
    }
    else
    {
        cout << "Error: Se esperaba un identificador o 'print', pero se encontró: " << *current << endl;
        exit(1);
    }
    return s;
}

Exp *Parser::parseCExp()
{
    Exp *left = parseExpression();
    if (match(Token::LT) || match(Token::LE) || match(Token::EQ))
    {
        BinaryOp op;
        if (previous->type == Token::LT)
        {
            op = LT_OP;
        }
        else if (previous->type == Token::LE)
        {
            op = LE_OP;
        }
        else if (previous->type == Token::EQ)
        {
            op = EQ_OP;
        }
        Exp *right = parseExpression();
        left = new BinaryExp(left, right, op);
    }
    return left;
}

Exp *Parser::parseExpression()
{
    Exp *left = parseTerm();
    while (match(Token::PLUS) || match(Token::MINUS))
    {
        BinaryOp op;
        if (previous->type == Token::PLUS)
        {
            op = PLUS_OP;
        }
        else if (previous->type == Token::MINUS)
        {
            op = MINUS_OP;
        }
        Exp *right = parseTerm();
        left = new BinaryExp(left, right, op);
    }
    return left;
}

Exp *Parser::parseTerm()
{
    Exp *left = parseFactor();
    while (match(Token::MUL) || match(Token::DIV))
    {
        BinaryOp op;
        if (previous->type == Token::MUL)
        {
            op = MUL_OP;
        }
        else if (previous->type == Token::DIV)
        {
            op = DIV_OP;
        }
        Exp *right = parseFactor();
        left = new BinaryExp(left, right, op);
    }
    return left;
}

Exp *Parser::parseFactor()
{
    Exp *e;
    Exp *e1;
    Exp *e2;
    if (match(Token::TRUE))
    {
        return new BoolExp(1);
    }
    else if (match(Token::FALSE))
    {
        return new BoolExp(0);
    }
    else if (match(Token::NUM))
    {
        return new NumberExp(stoi(previous->text));
    }
    else if (match(Token::ID))
    {
        string texto = previous->text;
        // Parse FCallExp
        if (match(Token::PI))
        {
            list<Exp *> args;
            if (!check(Token::PD))
            {
                args.push_back(parseCExp());
                while (match(Token::COMA))
                {
                    args.push_back(parseCExp());
                }
            }
            if (!match(Token::PD))
            {
                cout << "Falta paréntesis derecho" << endl;
                exit(0);
            }
            return new FCallExp(texto, args);
        }
        else
        {
            return new IdentifierExp(previous->text);
        }
    }
    else if (match(Token::IFEXP))
    {
        match(Token::PI);
        e = parseCExp();
        match(Token::COMA);
        e1 = parseCExp();
        match(Token::COMA);
        e2 = parseCExp();
        match(Token::PD);
        return new IFExp(e, e1, e2);
    }
    else if (match(Token::PI))
    {
        e = parseCExp();
        if (!match(Token::PD))
        {
            cout << "Falta paréntesis derecho" << endl;
            exit(0);
        }
        return e;
    }
    cout << "Error: se esperaba un número o identificador." << endl;
    exit(0);
}
