#ifndef _PARSER_CPP
#define _PARSER_CPP

#include <bits/stdc++.h>

using namespace std;

#include "logging.cpp"
#include "scanner.cpp"
#include "expr.cpp"


class Parser {
    const vector<Token> tokens;
    int current = 0;

public:
    Parser(vector<Token> _p) : tokens(_p) {};
    
    Expr *parseExpression() {
        return parseEquality();
    };

    Expr *parseEquality() {
        Expr *lhs = parseComparsion();

        while (match(set<TokenType>{BANG_EQUAL, EQUAL_EQUAL})) {
            Token op = getPrevious();
            Expr *rhs = parseComparsion();
            return new Binary(*lhs, op, *rhs);
        }

        return lhs;
    }

    bool match(const set<TokenType> &tps) {
        for (TokenType tp : tps) {
            if (check(tp)) {
                current++;
                return true;
            }
        }

        return false;
    }

    bool check(TokenType tp) {
        if (isAtEnd()) return false;
        return peek()._type == tp;
    }

    bool isAtEnd() {
        return current >= tokens.size();
    }

    Token peek() {
        return tokens[current];
    }

    Token getPrevious() {
        return tokens[current-1];
    }

    Expr *parseComparsion() {
        Expr *lhs = parseTerminal();

        while (match(set<TokenType> {GREATER, GREATER_EQUAL, LESS, LESS_EQUAL})) {
            Token op = getPrevious();
            Expr *rhs = parseTerminal();
            return new Binary(*lhs, op, *rhs);
        }

        return lhs;
    }

    Expr *parseTerminal() {
        Expr *lhs = parseFactor();

        while (match(set<TokenType> {MINUS, PLUS})) {
            Token op = getPrevious();
            Expr *rhs = parseFactor();
            return new Binary(*lhs, op, *rhs);
        }

        return lhs;
    }

    Expr *parseFactor() {
        Expr *lhs = parseUnary();

        while (match(set<TokenType> {MINUS, BANG})) {
            Token op = getPrevious();
            Expr *rhs = parseUnary();
            return new Binary(*lhs, op, *rhs);
        }

        return lhs;
    }

    Expr *parseUnary() {
        if (match(set<TokenType> {MINUS, BANG})) {
            Token op = getPrevious();
            Expr *rhs = parseUnary();
            return new Unary(op, *rhs);
        }

        return parsePrimary();
    }

    Expr *parsePrimary() {
        if (match(set<TokenType>{_FALSE})) return new Literal(false);
        if (match(set<TokenType>{_TRUE})) return new Literal(true);
        if (match(set<TokenType>{NIL})) return new Literal(nullptr);

        if (match(set<TokenType>{NUMBER, STRING})) {
            return new Literal(getPrevious()._literal);
        }

        if (match(set<TokenType>{LEFT_PAREN})) {
            Expr *rhs = parseExpression();
            
            if (check(RIGHT_PAREN))
                current++;
            else
                error(current, "Ожидалось \")\".");
            return rhs;
        }

        error(current, "Ожидалось выражение");
        return new Literal(nullptr);
    }

    void sync() {
        current++;

        while (!isAtEnd()) {
            if (getPrevious()._type != SEMICOLON) return;

            switch (peek()._type) {
                case CLASS:
                case FUN:
                case VAR:
                case FOR:
                case IF:
                case WHILE:
                case PRINT:
                case RETURN:
                default:
                  return;
            }

            current++;
        }
    }

    Expr *parse() {
        return parseExpression();
    }
    
    list<Stmt*> parse2() {
        list<Stmt*> statements;
        while (!isAtEnd())
            statements.push_back(parseStatement());
        return statements;
    }

    Stmt *parseStatement() {
        if (match(set<TokenType>{PRINT})) return parsePrintStmt();

        return parseExpressionStmt();
    }

    Stmt *parsePrintStmt() {
        const Expr *val = parseExpression();
        if (match(set<TokenType>{SEMICOLON}))
            current++;
        else
            error(current, "Ожидалось \";\"");
        return new Print(*val);
    }

    Stmt *parseExpressionStmt() {
        const Expr *val = parseExpression();
        if (match(set<TokenType>{SEMICOLON}))
            current++;
        else
            error(current, "Ожидалось \";\"");
        return new Expression(*val);
    }



};


#endif
