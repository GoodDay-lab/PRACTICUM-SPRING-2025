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
    
    Expr parseExpression() {
        return parseEquality();
    };

    Expr parseEquality() {
        Expr lhs = parseComprasion();

        while (match(set<TokenType>{BANG_EQUAL, EQUAL_EQUAL})) {
            Token op = getPrevious();
            Expr rhs = parseComprasion();
            lhs = Binary(lhs, op, rhs);
        }

        return lhs;
    }

    bool match(set<TokenType> tps) {
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

    Expr parseComprasion() {
        Expr lhs = parseTerminal();

        while (match(set<TokenType> {GREATER, GREATER_EQUAL, LESS, LESS_EQUAL})) {
            Token op = getPrevious();
            Expr rhs = parseTerminal();
            lhs = Binary(lhs, op, rhs);
        }

        return lhs;
    }

    Expr parseTerminal() {
        Expr lhs = parseFactor();

        while (match(set<TokenType> {MINUS, PLUS})) {
            Token op = getPrevious();
            Expr rhs = parseFactor();
            lhs = Binary(lhs, op, rhs);
        }

        return lhs;
    }

    Expr parseFactor() {
        Expr lhs = parseUnary();

        while (match(set<TokenType> {MINUS, BANG})) {
            Token op = getPrevious();
            Expr rhs = parseUnary();
            lhs = Binary(lhs, op, rhs);
        }

        return lhs;
    }

    Expr parseUnary() {
        if (match(set<TokenType> {MINUS, BANG})) {
            Token op = getPrevious();
            Expr rhs = parseUnary();
            return Unary(op, rhs);
        }

        return parsePrimary();
    }

    Expr parsePrimary() {
        if (match(set<TokenType>{_FALSE})) return Literal(false);
        if (match(set<TokenType>{_TRUE})) return Literal(true);
        if (match(set<TokenType>{NIL})) return Literal(nullptr);

        if (match(set<TokenType>{NUMBER, STRING})) {
            return Literal(getPrevious()._literal);
        }

        if (match(set<TokenType>{LEFT_PAREN})) {
            Expr rhs = parseExpression();
            
            if (check(RIGHT_PAREN))
                return Group(rhs);
            error(current, "Ожидалось \")\".");
        }
        return Literal(nullptr);
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

    Expr parse() {
        return parseExpression();
    }
};


#endif
