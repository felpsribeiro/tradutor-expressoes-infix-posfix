#include "parser.h"
#include "error.h"
#include <iostream>
#include <sstream>
#include <cctype>
using std::cin;
using std::cout;
using std::endl;
using std::stringstream;

void Parser::Program()
{
    // program -> math block
    if (Match(Tag::FUNC_INI))
        Block();
    else
        throw SyntaxError(scanner.Lineno(), "\'math\' esperado");
}

void Parser::Block()
{
    // block -> { decls stmts }
    if (!Match('{'))
        throw SyntaxError(scanner.Lineno(), "\'{\' esperado");

    // nova tabela de símbolos para o bloco
    // ------------------------------------
    SymTable *saved = symtable;
    symtable = new SymTable(symtable);
    // ------------------------------------

    Decls();
    Stmts();

    if (!Match('}'))
        throw SyntaxError(scanner.Lineno(), "\'}\' esperado");

    // tabela do escopo envolvente volta a ser a tabela ativa
    // ------------------------------------------------------
    delete symtable;
    symtable = saved;
    // ------------------------------------------------------
}

void Parser::Decls()
{
    // decls -> decl decls
    //        | empty
    // decl  -> type id;

    while (lookahead->tag == Tag::TYPE)
    {
        string type{lookahead->toString()};
        Match(Tag::TYPE);

        string name{lookahead->toString()};
        if (!Match(Tag::ID))
        {
            stringstream ss;
            if (lookahead->tag == Tag::TYPE || lookahead->tag == Tag::FUNC_INI)
                ss << "não é possivel usar a palavra \'" << lookahead->toString() << "\' como nome de variavel pois ela é uma palavra reservada do tradutor";
            else
                ss << "nome de variavel inválido, use apenas letras do alfabetos";
            throw SyntaxError(scanner.Lineno(), ss.str());
        }

        Symbol s{name, type};

        // insere variável na tabela de símbolos
        if (!symtable->Insert(name, s))
        {
            // a inserção falha quando a variável já está na tabela
            stringstream ss;
            ss << "variável \"" << name << "\" já definida";
            throw SyntaxError(scanner.Lineno(), ss.str());
        }

        if (!Match(';'))
        {
            stringstream ss;
            ss << "encontrado \'" << lookahead->toString() << "\' no lugar de ';'";
            throw SyntaxError{scanner.Lineno(), ss.str()};
        }
    }
}

void Parser::Stmts()
{
    // stmts -> stmt stmts
    //        | empty
    // stmt  -> block
    //        | expr;

    while (true)
    {
        switch (lookahead->tag)
        {
        // stmt -> block
        case '{':
            Block();
            break;
        // stmt -> expr;
        case '(':
        case Tag::NUM:
        case Tag::ID:
            Expr();
            if (!Match(';'))
            {
                stringstream ss;
                ss << "encontrado \'" << lookahead->toString() << "\' no lugar de ';'";
                throw SyntaxError{scanner.Lineno(), ss.str()};
            }
            else
                cout << endl;
            break;
        // stmts -> empty
        default:
            return;
        }
    }
}

void Parser::Expr()
{
    // expr -> term paexp
    // opexpr -> + term opexpr
    //        -> - term opexpr
    //        -> empty
    Term();
    while (true)
    {
        switch (lookahead->tag)
        {
        // opexpr -> + term opexpr
        case '+':
            Match('+');
            Term();
            cout << '+';
            break;
        // opexpr -> - term opexpr
        case '-':
            Match('-');
            Term();
            cout << '-';
            break;
        // opexpr -> empty
        default:
            return;
        }
    }
}

void Parser::Term()
{
    // term -> factor opterm
    // opterm -> * factor opterm
    //         | / factor opterm
    //         | empty
    Factor();
    while (true)
    {
        switch (lookahead->tag)
        {
        // opterm -> * factor opterm
        case '*':
            Match('*');
            Factor();
            cout << '*';
            break;
        // opterm -> / factor opterm
        case '/':
            Match('/');
            Factor();
            cout << '/';
            break;
        // opterm -> empty
        default:
            return;
        }
    }
}

void Parser::Factor()
{
    // factor -> (expr)
    //         | num
    //         | id
    switch (lookahead->tag)
    {
    // factor -> (expr)
    case '(':
        Match('(');
        Expr();
        if (!Match(')'))
            throw SyntaxError(scanner.Lineno(), "\')\' esperado");
        break;
    // factor -> num
    case Tag::NUM:
        cout << '(' << lookahead->toString() << ')';
        Match(Tag::NUM);
        break;
    // factor -> id
    case Tag::ID:
    {
        Symbol *s = symtable->Find(lookahead->toString());
        if (!s)
        {
            stringstream ss;
            ss << "variável \"" << lookahead->toString() << "\" não declarada";
            throw SyntaxError{scanner.Lineno(), ss.str()};
        }
        cout << '(' << s->var << ':' << s->type << ")";
        Match(Tag::ID);
        break;
    }
    default:
        stringstream ss;
        ss << '\'' << lookahead->toString() << "\' inválido na expressão";
        throw SyntaxError{scanner.Lineno(), ss.str()};
    }
}

bool Parser::Match(int tag)
{
    if (tag == lookahead->tag)
    {
        lookahead = scanner.Scan();
        return true;
    }

    return false;
}

Parser::Parser()
{
    lookahead = scanner.Scan();
    symtable = nullptr;
}

void Parser::Start()
{
    Program();
}
