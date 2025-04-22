#ifndef _EXPR_CPP
#define _EXPR_CPP

#include <bits/stdc++.h>

using namespace std;

#include "logging.cpp"
#include "scanner.cpp"

class Expr {
    ;
};

class Binary : public Expr {
    const Expr lhs, rhs;
    const Token op;
public:
    Binary(Expr _lhs, Token _op, Expr _rhs) :
        lhs(_lhs), rhs(_rhs), op(_op) {};
};

class Group : public Expr {
    const Expr expr;
public:
    Group(Expr _p) : expr(_p) {};
};

class Literal : public Expr {
    const LiteralType lit;
public:
    Literal(LiteralType _p) : lit(_p) {};
};

class Unary : public Expr {
    const Expr rhs;
    const Token op;
public:
    Unary(Token _op, Expr _p) : rhs(_p), op(_op) {};
};

class ASTView {
public:
    string print(Expr expr) {
        return "printing...";
    };
};


#endif
