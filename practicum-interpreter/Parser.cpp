#include "Parser.h"

#include <exception>
#include <functional>
#include <initializer_list>
#include <memory>
#include <optional>
#include <type_traits>
#include <variant>
#include <vector>

#include "DebugPrint.h"

namespace cpplox::Parser {

using Types::Token;
using Types::TokenType;

RDParser::RDParser(const std::vector<Token>& p_tokens,
                   ErrorsAndDebug::ErrorReporter& eReporter)
    : tokens(p_tokens), eReporter(eReporter) {
  this->currentIter = this->tokens.begin();
}

// Helper functions; Sorted by name
void RDParser::advance() {
  if (!isAtEnd()) ++currentIter;
}

auto RDParser::consumeAnyBinaryExprs(
    const std::initializer_list<TokenType>& types, ExprPtrVariant expr,
    const parserFn& f) -> ExprPtrVariant {
  while (match(types)) {
    Token op = getTokenAndAdvance();
    expr = AST::createBinaryEPV(std::move(expr), op, std::invoke(f, this));
  }
  return expr;
}

void RDParser::consumeOrError(TokenType tType,
                              const std::string& errorMessage) {
  if (getCurrentTokenType() == tType) return advance();
  throw error(errorMessage + " Got: " + peek().toString());
}

auto RDParser::consumeOneLiteral(const std::string& str) -> ExprPtrVariant {
  advance();
  return AST::createLiteralEPV(Types::makeOptionalLiteral(str));
}

auto RDParser::consumeOneLiteral() -> ExprPtrVariant {
  return AST::createLiteralEPV(getTokenAndAdvance().getOptionalLiteral());
}

auto RDParser::consumeGroupingExpr() -> ExprPtrVariant {
  advance();
  ExprPtrVariant expr = expression();
  consumeOrError(TokenType::RIGHT_PAREN,
                 std::to_string(peek().getLine())
                     + " Expected a closing paren after expression.");
  return AST::createGroupingEPV(std::move(expr));
}

auto RDParser::consumeUnaryExpr() -> ExprPtrVariant {
  return AST::createUnaryEPV(getTokenAndAdvance(), unary());
}

auto RDParser::consumeVarExpr() -> ExprPtrVariant {
  Token varName = getTokenAndAdvance();
  return AST::createVariableEPV(varName);
}

void RDParser::consumeSemicolonOrError() {
  consumeOrError(TokenType::SEMICOLON, "Expected a ';'");
}

auto RDParser::error(const std::string& eMessage) -> RDParser::RDParseError {
  reportError(eMessage);
  return RDParser::RDParseError();
}

auto RDParser::getCurrentTokenType() const -> TokenType {
  return currentIter->getType();
}

auto RDParser::getTokenAndAdvance() -> Token {
  Token token = peek();
  advance();
  return token;
}

auto RDParser::isAtEnd() const -> bool {
  return peek().getType() == TokenType::LOX_EOF;
}

auto RDParser::match(Types::TokenType type) const -> bool {
  if (isAtEnd()) return false;
  return (type == getCurrentTokenType());
}

auto RDParser::match(const std::initializer_list<Types::TokenType>& types) const
    -> bool {
  bool result = false;
  for (const auto& type : types) result = (result || match(type));
  return result;
}

auto RDParser::matchNext(Types::TokenType type) -> bool {
  advance();
  bool result = match(type);
  --currentIter;
  return result;
}

auto RDParser::peek() const -> Token { return *currentIter; }

void RDParser::reportError(const std::string& message) {
  const Token& token = peek();
  std::string error = message;
  if (token.getType() == TokenType::LOX_EOF)
    error = " at end: " + error;
  else
    error = " at '" + token.getLexeme() + "': " + error;
  eReporter.setError(token.getLine(), error);
}

void RDParser::synchronize() {
  while (!isAtEnd()) {
    switch (getCurrentTokenType()) {
      case TokenType::SEMICOLON: advance(); return;
      case TokenType::INTW:
      case TokenType::STRINGW:
      case TokenType::REALW:
      case TokenType::FOR:
      case TokenType::IF:
      case TokenType::WHILE:
      case TokenType::WRITE:
      case TokenType::READ: return;
      default:
        ErrorsAndDebug::debugPrint("Discarding extranuous token:"
                                   + peek().getLexeme());
        advance();
    }
  }
}

void RDParser::throwOnErrorProduction(
    const std::initializer_list<Types::TokenType>& types, const parserFn& f) {
  if (match(types)) {
    auto errObj = error("Missing left hand operand");
    advance();
    ExprPtrVariant expr
        = std::invoke(f, this);  // We check the rest of the expression anyways
                                 // to see if there are any errors there
    throw errObj;
  }
}

void RDParser::throwOnErrorProductions() {
  throwOnErrorProduction({TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL},
                         &RDParser::equality);
  throwOnErrorProduction({TokenType::GREATER, TokenType::GREATER_EQUAL,
                          TokenType::LESS, TokenType::LESS_EQUAL},
                         &RDParser::comparison);
  throwOnErrorProduction({TokenType::PLUS}, &RDParser::addition);
  throwOnErrorProduction({TokenType::STAR, TokenType::SLASH, TokenType::MOD},
                         &RDParser::multiplication);
}

// ---------------- Grammar Production Rules -----------------------------------
// This is a recursive descent parser. The grammar for Lox is declared above
// each of the functions. The functions are sorted based on lowest to highest
// precedence;

// program     → declaration* LOX_EOF;
void RDParser::program() {
  try {
    consumeOrError(TokenType::PROGRAM, "Expected program keyword");
    consumeOrError(TokenType::LEFT_BRACE, "Expected \'{\' bracket");
    while (!isAtEnd() && match({TokenType::INTW, TokenType::STRINGW, TokenType::REALW})) {
      declaration();
    }

    while (!isAtEnd() && !match(TokenType::RIGHT_BRACE)) {
      std::optional<StmtPtrVariant> optStmt = std::make_optional(statement());
      if (optStmt.has_value())
        statements.push_back(std::move(optStmt.value()));
    }
    consumeOrError(TokenType::RIGHT_BRACE, "Expected \'}\' bracket");
  } catch (const std::exception& e) {
    std::string errorMessage = "Caught unhandled exception: ";
    errorMessage += e.what();
    eReporter.setError(peek().getLine(), errorMessage);
  } catch (const RDParseError& e) {
    std::string errorMessage = "Caught unhandled parse error: ";
    eReporter.setError(peek().getLine(), errorMessage);
  }
}
// declaration → intDecl | strDecl | realDecl ;
auto RDParser::declaration() -> std::optional<StmtPtrVariant> {
  try {
    if (match(TokenType::STRINGW)) {
      advance();
      strDecl();
    }

    if (match(TokenType::INTW)) {
      advance();
      intDecl();
    }

    if (match(TokenType::REALW)) {
      advance();
      realDecl();
    }

    return std::nullopt;
  } catch (const RDParseError& e) {
    ErrorsAndDebug::debugPrint(
        "Caught ParseError; Calling synchronize. Current Token:"
        + peek().toString());
    synchronize();
    return std::nullopt;
  }
}

// strDecl     → "string" IDENTIFIER ("=" expression)? ";" ;
auto RDParser::strDecl() -> void {
  do {
    if (match(TokenType::COMMA))
      advance();
    if (match(TokenType::IDENTIFIER)) {
      Token varName = getTokenAndAdvance();
      std::optional<ExprPtrVariant> intializer = std::nullopt;
      if (match(TokenType::EQUAL)) {
        advance();
        intializer = assignment();
      }
      statements.push_back(AST::createStrSPV(varName, std::move(intializer)));
    } else
      throw error("Expected a variable name after the str keyword");
  } while (match(TokenType::COMMA));
  consumeSemicolonOrError();
}

// intDecl     → "int" IDENTIFIER ("=" expression)? ";" ;
auto RDParser::intDecl() -> void {
  do {
    if (match(TokenType::COMMA))
      advance();
    if (match(TokenType::IDENTIFIER)) {
      Token varName = getTokenAndAdvance();
      std::optional<ExprPtrVariant> intializer = std::nullopt;
      if (match(TokenType::EQUAL)) {
        advance();
        intializer = assignment();
      }
      statements.push_back(AST::createIntSPV(varName, std::move(intializer)));
    } else 
      throw error("Expected a variable name after the int keyword");
  } while (match(TokenType::COMMA));
  consumeSemicolonOrError();
}

// realDecl     → "real" IDENTIFIER ("=" expression)? ";" ;
auto RDParser::realDecl() -> void {
  do {
    if (match(TokenType::COMMA))
      advance();
    if (match(TokenType::IDENTIFIER)) {
      Token varName = getTokenAndAdvance();
      std::optional<ExprPtrVariant> intializer = std::nullopt;
      if (match(TokenType::EQUAL)) {
        advance();
        intializer = assignment();
      }
      statements.push_back(AST::createRealSPV(varName, std::move(intializer)));
    } else
      throw error("Expected a variable name after the real keyword");
  } while (match(TokenType::COMMA));
  consumeSemicolonOrError();
}

// statement   → exprStmt | writeStmt | readStmt | blockStmt | ifStmt | whileStmt |
// statement   → forStmt;
auto RDParser::statement() -> StmtPtrVariant {
  if (match(TokenType::WRITE)) return writeStmt();
  if (match(TokenType::READ)) return readStmt();
  if (match(TokenType::LEFT_BRACE)) return blockStmt();
  if (match(TokenType::IF)) return ifStmt();
  if (match(TokenType::WHILE)) return whileStmt();
  if (match(TokenType::FOR)) return forStmt();
  if (match(TokenType::BREAK)) return breakStmt();
  return exprStmt();
}

// exprStmt    → expression ';' ;
auto RDParser::exprStmt() -> StmtPtrVariant {
  auto expr = expression();
  consumeSemicolonOrError();
  return AST::createExprSPV(std::move(expr));
}

// writeStmt   → "write(<expression>, ...);" ;
auto RDParser::writeStmt() -> StmtPtrVariant {
  advance();
  std::vector<ExprPtrVariant> exprs;
  consumeOrError(TokenType::LEFT_PAREN, "Expected \'(\'"); 
  ExprPtrVariant printExpr = assignment();
  exprs.push_back(std::move(printExpr));
  while (match(TokenType::COMMA)) {
    advance();
    printExpr = assignment();
    exprs.push_back(std::move(printExpr));
  }
  consumeOrError(TokenType::RIGHT_PAREN, "Expected \')\'");
  consumeSemicolonOrError();
  return AST::createWriteSPV(std::move(exprs));
}

// readStmt   → "read(<identifier>);" ;
auto RDParser::readStmt() -> StmtPtrVariant {
  advance();
  consumeOrError(TokenType::LEFT_PAREN, "Expected \'(\'");
  consumeOrError(TokenType::IDENTIFIER, "Expected a variable name");
  --currentIter;
  Token name = getTokenAndAdvance();
  consumeOrError(TokenType::RIGHT_PAREN, "Expected \')\'");
  consumeSemicolonOrError();
  return AST::createReadSPV(name);
}

// blockStmt   → "{" declaration "}"
auto RDParser::blockStmt() -> StmtPtrVariant {
  // Do something special for scopes;
  advance();  // consume '{'
  std::vector<StmtPtrVariant> statements;
  while (!match(TokenType::RIGHT_BRACE) && !isAtEnd()) {
    auto optStmnt = statement();
    statements.push_back(std::move(optStmnt));
  }
  consumeOrError(TokenType::RIGHT_BRACE, "Expect '}' after block");
  return AST::createBlockSPV(std::move(statements));
}

// ifStmt      → "if" "(" expression ")" statement ("else" statement)? ;
auto RDParser::ifStmt() -> StmtPtrVariant {
  advance();
  consumeOrError(TokenType::LEFT_PAREN, "Expecte '(' after if.");
  ExprPtrVariant condition = expression();
  consumeOrError(TokenType::RIGHT_PAREN, "Expecte ')' after if condition.");

  StmtPtrVariant thenBranch = statement();

  std::optional<StmtPtrVariant> elseBranch = std::nullopt;
  if (match(TokenType::ELSE)) {
    advance();
    elseBranch = std::make_optional(statement());
  }

  return AST::createIfSPV(std::move(condition), std::move(thenBranch),
                          std::move(elseBranch));
}

// whileStmt   → "while" "(" expression ")" statement;
auto RDParser::whileStmt() -> StmtPtrVariant {
  advance();
  consumeOrError(TokenType::LEFT_PAREN, "Expecte '(' after while.");
  ExprPtrVariant condition = expression();
  consumeOrError(TokenType::RIGHT_PAREN, "Expecte ')' after while condition.");
  return AST::createWhileSPV(std::move(condition), statement());
}

// forStmt     → "for" "("
//                          (varDecl | exprStmnt | ";")
//                          expression? ";"
//                          expression?
//                     ")"
//                statement;
auto RDParser::forStmt() -> StmtPtrVariant {
  advance();
  consumeOrError(TokenType::LEFT_PAREN, "Expected '(' after for.");

  std::optional<StmtPtrVariant> initializer = std::nullopt;
  if (match(TokenType::SEMICOLON)) {
    advance();
  } else {
    initializer = std::make_optional(exprStmt());
  }

  std::optional<ExprPtrVariant> condition = std::nullopt;
  if (!match(TokenType::SEMICOLON))
    condition = std::make_optional(expression());

  consumeSemicolonOrError();

  std::optional<ExprPtrVariant> increment = std::nullopt;
  if (!match(TokenType::RIGHT_PAREN))
    increment = std::make_optional(expression());

  consumeOrError(TokenType::RIGHT_PAREN, "Expecte ')' after 'for' clauses.");

  StmtPtrVariant loopBody = statement();

  return AST::createForSPV(std::move(initializer), std::move(condition),
                           std::move(increment), std::move(loopBody));
}

// breakStmt
auto RDParser::breakStmt() -> StmtPtrVariant {
  Token name = peek();
  advance();
  consumeSemicolonOrError();
  return AST::createBreakSPV(name);
}

//=============//
// Expressions //
//=============//
// expression → comma;
auto RDParser::expression() -> ExprPtrVariant { return comma(); }

// !This is going to cause some grief when we are looking at function
// arguments; !fun(1,2) will be treated as fun((1,2)) <- a single argument
// instead of two. !Needs to make sure we set function arguments to have
// higher precedence than !the comma operator
// comma → assignment ("," assignment)*;
auto RDParser::comma() -> ExprPtrVariant {
  return consumeAnyBinaryExprs({TokenType::COMMA}, assignment(),
                               &RDParser::assignment);
}

// assignment  → IDENTIFIER "=" assignment | condititional;
auto RDParser::assignment() -> ExprPtrVariant {
  ExprPtrVariant expr = conditional();

  if (match(TokenType::EQUAL)) {
    advance();
    if (std::holds_alternative<AST::VariableExprPtr>(expr)) {
      Token varName = std::get<AST::VariableExprPtr>(expr)->varName;
      return AST::createAssignmentEPV(varName, assignment());
    }
    throw error("Invalid assignment target");
  }

  return expr;
}

// conditional → logical_or ("?" expression ":" conditional)?;
// conditional → equality ("?" expression ":" conditional)?
auto RDParser::conditional() -> ExprPtrVariant {
  ExprPtrVariant expr = logical_or();
  if (match(TokenType::QUESTION)) {
    Token op = getTokenAndAdvance();
    ExprPtrVariant thenBranch = expression();
    consumeOrError(TokenType::COLON, "Expected a colon after ternary operator");
    return AST::createConditionalEPV(std::move(expr), std::move(thenBranch),
                                     conditional());
  }
  return expr;
}

// logical_or  → logical_and ("or" logical_and)*;
auto RDParser::logical_or() -> ExprPtrVariant {
  ExprPtrVariant expr = logical_and();
  while (match(TokenType::OR)) {
    Token op = getTokenAndAdvance();
    expr = AST::createLogicalEPV(std::move(expr), op, logical_and());
  }
  return expr;
}

// logical_and → equality ("and" equality)*;
auto RDParser::logical_and() -> ExprPtrVariant {
  ExprPtrVariant expr = equality();
  while (match(TokenType::AND)) {
    Token op = getTokenAndAdvance();
    expr = AST::createLogicalEPV(std::move(expr), op, equality());
  }
  return expr;
}

// equality   → comparison(("!=" | "==") comparison) *;
auto RDParser::equality() -> ExprPtrVariant {
  auto equalityTypes = {TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL};
  return consumeAnyBinaryExprs(equalityTypes, comparison(),
                               &RDParser::comparison);
}

// comparison → addition((">" | ">=" | "<" | "<=") addition) *;
auto RDParser::comparison() -> ExprPtrVariant {
  auto comparatorTypes = {TokenType::GREATER, TokenType::GREATER_EQUAL,
                          TokenType::LESS, TokenType::LESS_EQUAL};
  return consumeAnyBinaryExprs(comparatorTypes, addition(),
                               &RDParser::addition);
}

// addition   → multiplication(("-" | "+") multiplication) *;
auto RDParser::addition() -> ExprPtrVariant {
  auto additionTypes = {TokenType::PLUS, TokenType::MINUS};
  return consumeAnyBinaryExprs(additionTypes, multiplication(),
                               &RDParser::multiplication);
}

// multi...   → unary(("/" | "*" | "%") unary) *;
auto RDParser::multiplication() -> ExprPtrVariant {
  auto multTypes = {TokenType::SLASH, TokenType::STAR, TokenType::MOD};
  return consumeAnyBinaryExprs(multTypes, unary(), &RDParser::unary);
}

// unary      → ("!" | "-" | "--" | "++") unary | primary;
auto RDParser::unary() -> ExprPtrVariant {
  auto unaryTypes = {TokenType::BANG, TokenType::MINUS};
  if (match(unaryTypes)) return consumeUnaryExpr();
  return primary();
}

// primary    → NUMBER | STRING | "false" | "true" | "nil";
// primary    →  "(" expression ")" | IDENTIFIER;
//  Error Productions:
// primary    → ("!=" | "==") equality
// primary    → (">" | ">=" | "<" | "<=") comparison
// primary    → ("+")addition
// primary → ("/" | "*" | "%") multiplication;
auto RDParser::primary() -> ExprPtrVariant {
  if (match(TokenType::LOX_FALSE)) return consumeOneLiteral("false");
  if (match(TokenType::LOX_TRUE)) return consumeOneLiteral("true");
  if (match(TokenType::NIL)) return consumeOneLiteral("nil");
  if (match(TokenType::NUMBER)) return consumeOneLiteral();
  if (match(TokenType::STRING)) return consumeOneLiteral();
  if (match(TokenType::LEFT_PAREN)) return consumeGroupingExpr();
  if (match(TokenType::IDENTIFIER)) return consumeVarExpr();

  // Check for known error productions. throws RDParseError;
  throwOnErrorProductions();

  // if none of the known error productions match,
  // give up and throw generic RDParseError;
  throw error("Expected an expression; Got something else.");
}

// _________

auto RDParser::parse() -> std::vector<StmtPtrVariant> {
  program();
  return std::move(this->statements);
}

}  // namespace cpplox::Parser
