#ifndef _SCANNER_CPP
#define _SCANNER_CPP

#include <bits/stdc++.h>

#include "logging.cpp"

using namespace std;

enum TokenType 
{
  LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE,
  COMMA, DOT, MINUS, PLUS, SEMICOLON, SLASH, STAR, MOD,

  BANG, BANG_EQUAL,
  EQUAL, EQUAL_EQUAL,
  GREATER, GREATER_EQUAL,
  LESS, LESS_EQUAL,

  IDENTIFIER, STRING, NUMBER, PROGRAM,

  AND, CLASS, ELSE, _FALSE, FUN, FOR, IF, NIL, OR, UNTIL, GOTO, DO,
  PRINT, RETURN, SUPER, THIS, _TRUE, VAR, WHILE, BREAK, CONTINUE,

  READ, WRITE,

  _EOF
};

static map<string, TokenType> keywords;

void initKeywords() {
    keywords["and"] = AND;
    keywords["class"] = CLASS;
    keywords["else"] =  ELSE;
    keywords["false"] = _FALSE;
    keywords["for"] =   FOR;
    keywords["if"] =    IF;
    keywords["nil"] =   NIL;
    keywords["or"] =  OR;
    keywords["print"] = PRINT;
    keywords["return"] = RETURN;
    keywords["super"] = SUPER;
    keywords["this"] =  THIS;
    keywords["true"] =  _TRUE;
    keywords["var"] =   VAR;
    keywords["while"] = WHILE;
    keywords["continue"] = CONTINUE;
    keywords["break"] = BREAK;
    keywords["read"] = READ;
    keywords["write"] = WRITE;
    keywords["program"] = PROGRAM;
    keywords["goto"] = GOTO;
    keywords["until"] = UNTIL;
    keywords["do"] = DO;
}

typedef variant<int, double, nullptr_t, string> LiteralType;

class Token 
{
    const TokenType _type;
    const string _lexeme;
    const int _line;
    const LiteralType _literal;

public:
    Token(TokenType _t, string _l, LiteralType _lit, int _n) : 
      _type(_t), _lexeme(_l), _line(_n), _literal(_lit) {};
    ~Token() {};

    string toString() const {
        return to_string(_type) + " " + _lexeme + " " + to_string(_line);
    }
};

class Scanner
{
    string source;
    vector<Token> tokens;
    bool isAtEnd;

    int start = 0;
    int current = 0;
    int line = 1;

public:
    explicit Scanner(const string& _src) {
        this->source = _src;
        isAtEnd = false;
    };
    ~Scanner() {};

    vector<Token> scanTokens() {
        while (!(isAtEnd = (current >= source.size()))) {
            start = current;
            scanToken();
        }

        tokens.push_back(Token(_EOF, "", nullptr, line));
        return tokens;
    };

    void scanToken() {
        char c = source[current++];
        switch (c) {
          case '(': addToken(LEFT_PAREN); break;
          case ')': addToken(RIGHT_PAREN); break;
          case '{': addToken(LEFT_BRACE); break;
          case '}': addToken(RIGHT_BRACE); break;
          case ',': addToken(COMMA); break;
          case '.': addToken(DOT); break;
          case '%': addToken(MOD); break;
          case '-':
            if (isdigit(peek())) {
              current++;
              scanNumber(1);
            } else
              addToken(MINUS);
            break;
          case '+': 
            if (isdigit(peek())) {
              current++;
              scanNumber(0);
            } else
              addToken(PLUS);
            break;
          case ';': addToken(SEMICOLON); break;
          case '*': addToken(STAR); break; 
          case '!':
            addToken(match('=') ? BANG_EQUAL : BANG);
            break;
          case '=':
            addToken(match('=') ? EQUAL_EQUAL : EQUAL);
            break;
          case '<':
            addToken(match('=') ? LESS_EQUAL : LESS);
            break;
          case '>':
            addToken(match('=') ? GREATER_EQUAL : GREATER);
            break;
          case '/':
            if (match('/')) {
              while (peek() != '\n' && !(isAtEnd = (current >= source.size()))) current++;
            } else {
              addToken(SLASH);
            }
            break;
          case ' ':
          case '\r':
          case '\t':
            break;
          case '\n':
            line += 1;
            break;
          case '"':
            scanString();
            break;
          default:
            if (isdigit(c)) {
              scanNumber();
            } else if (isalpha(c)) {
              scanIdentifier();
            } else
              error(line, "Неизвестный символ.");
            break;
        }    
    };

    void scanIdentifier() {
        while (isalpha(peek())) current++;

        string value = source.substr(start, current - start);
        if (keywords.find(value) == keywords.end())
            addToken(IDENTIFIER, value);
        else
            addToken(keywords[value]);
    };

    void scanNumber(int sign = 0) {
        while (isdigit(peek())) current++;

        if (peek() == '.' && isdigit(peekNext())) {
            current++;

            while (isdigit(peek())) current++;
        }

        if (sign)
          addToken(NUMBER, -stod(source.substr(start, current - start)));
        else
          addToken(NUMBER, stod(source.substr(start, current - start)));
    };

    void scanString() {
        while (peek() != '"' && !(isAtEnd = (current >= source.size()))) {
            if (peek() == '"') line++;
            current++;
        }

        if (isAtEnd) {
            error(line, "Незаконченная строка.");
            return;
        }

        current++;

        addToken(STRING, source.substr(start + 1, current - start - 2));
    };

    char peek() {
        if (isAtEnd) return '\x00';
        return source[current];
    };

    char peekNext() {
        if (current + 1 >= source.size()) return '\x00';
        return source[current+1];
    };

    bool match(char c) {
        if (isAtEnd) return false;
        if (c != source[current]) return false;

        current++;
        return true;
    };

    void addToken(TokenType tok) {
        addToken(tok, nullptr);
    };

    void addToken(TokenType tok, LiteralType lit) {
        tokens.push_back(Token(tok, source.substr(start, current - start), lit, line));
    };
};

ostream& operator<< (ostream& _cout, const Token tok) {
    cout << tok.toString() << '\n';
    return _cout;
}

#endif
