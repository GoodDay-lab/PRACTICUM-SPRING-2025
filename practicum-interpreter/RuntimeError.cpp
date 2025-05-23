#include "RuntimeError.h"

namespace cpplox::ErrorsAndDebug {

auto reportRuntimeError(ErrorReporter& eReporter, const Token& token,
                        const std::string& message) -> RuntimeError {
  eReporter.setError(token.getLine(), token.getLexeme() + ": " + message);
  return RuntimeError();
}

}  // namespace cpplox::ErrorsAndDebug
