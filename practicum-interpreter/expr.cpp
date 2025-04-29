#ifndef _EXPR_CPP
#define _EXPR_CPP

#include <bits/stdc++.h>

using namespace std;

#include "logging.cpp"
#include "scanner.cpp"

enum ExprType {
    BINARY, GROUP,
    LITERAL, UNARY, VARIABLE, ASSIGNEXPR
};

enum StmtType {
    EXPRESSION, PRINTSTMT, VARSTMT, IFSTMT, WHILESTMT, FORSTMT, SKIP
};

class Binary;
class Group;
class Literal;
class Unary;
class Variable;
class Assign;
typedef variant<Assign, Binary, Group, Literal, Unary, Variable> ExprClass;

class Expression;
class Print;
class Var;
class If;
class While;
class For;
class Skip;
typedef variant<Expression, Print, Var, If, While, For, Skip> StmtClass;

class Expr {
public:
    ExprType type;
    Expr(ExprType _t) : type(_t) {};
    virtual string accept() const = 0;
};

class Binary : public Expr {
    const Expr &lhs, &rhs;
    const Token op;
public:
    Binary(Expr &_lhs, Token _op, Expr &_rhs) : Expr(BINARY),
        lhs(_lhs), rhs(_rhs), op(_op) {};
    string accept() const override {
        return "(" + op._lexeme + " " + lhs.accept() + " " + rhs.accept() + ")";
    }
};

class Group : public Expr {
    const Expr &expr;
public:
    Group(Expr &_p) : Expr(GROUP), expr(_p) {};
    string accept() const override {
        return "(" + expr.accept() + ")";
    }
};

class Literal : public Expr {
    const LiteralType lit;
public:
    Literal(LiteralType _p) : Expr(LITERAL), lit(_p) {};
    string accept() const override {
        switch (lit.index()) {
            case 0:
                return to_string(get<0>(lit));
            case 1:
                return to_string(get<1>(lit));
            case 2:
                return "null";
            case 3:
                return get<3>(lit);
            default:
                return to_string(get<4>(lit));
        }
    }
};

class Unary : public Expr {
    const Expr &rhs;
    const Token op;
public:
    Unary(Token _op, Expr &_p) : Expr(UNARY), rhs(_p), op(_op) {};
    string accept() const override {
        return "(" + op._lexeme + " " + rhs.accept() + ")";
    }
};

class Variable : public Expr {
public:
    const Token name;
    Variable(Token _op) : Expr(VARIABLE), name(_op) {};
    string accept() const override {
        return name._lexeme;
    }
};

class Assign : public Expr {
    const Token name;
    const Expr &rhs;
public:
    Assign(Token n, Expr &e) : Expr(ASSIGNEXPR), name(n), rhs(e) {};
    string accept() const override {
        return "(= " + name._lexeme + " " + rhs.accept() + ")";
    }
};

class Stmt {
public:
    const StmtType type;
    Stmt(StmtType t) : type(t) {};
    virtual string accept() const = 0;
};

class Expression : public Stmt {
public:
    const Expr &expr;
    Expression(const Expr &e) : Stmt(EXPRESSION), expr(e) {};
    string accept() const override {
        return "(" + expr.accept() + ")";
    }
};

class Print : public Stmt {
public:
    const Expr &expr;
    Print(const Expr &e) : Stmt(PRINTSTMT), expr(e) {};
    string accept() const override {
        return "(print " + expr.accept() + ")";
    }
};

class Var : public Stmt {
public:
    const Token ident;
    const Expr &expr;
    Var(const Token t, const Expr &e) : Stmt(VARSTMT), ident(t), expr(e) {};
    string accept() const override {
        return "(var " + ident._lexeme + " " + expr.accept() + ")";
    }
};

class If : public Stmt {
public:
    const Expr &cond;
    const Stmt &lb;
    If(const Expr &e, const Stmt &l) : Stmt(IFSTMT),
    cond(e), lb(l) {};
    string accept() const override {
        return "(If " + cond.accept() + " ? " + lb.accept() + ")";
    }
};

class While : public Stmt {
public:
    const Expr &cond;
    const Stmt &stmt;
    While(const Expr &e, const Stmt &s) : Stmt(WHILESTMT), cond(e), stmt(s) {};
    string accept() const override {
        return "(while " + cond.accept() + " do " + stmt.accept() + ")";
    }
};

class For : public Stmt {
public:
    const Stmt &e1;
    const Expr &e2, &e3;
    const Stmt &stmt;
    For(const Stmt &e1, const Expr &e2, const Expr &e3, const Stmt &stmt) :
        Stmt(FORSTMT), e1(e1), e2(e2), e3(e3), stmt(stmt) {};
    string accept() const override {
        return "(for " + e1.accept() + "; " + e2.accept() + "; " + e3.accept() + " do " +
            stmt.accept() + ")";
    }
};

class Skip : public Stmt {
public:
    Skip() : Stmt(SKIP) {};
    string accept() const override {
        return "";
    }
};

class ASTView {
public:
    string print(const list<Stmt*> &stmts) {
        string ret = "";
        for (const Stmt *stmt : stmts)
            ret += stmt->accept() + "\n";
        return ret;
    };
};


#endif
