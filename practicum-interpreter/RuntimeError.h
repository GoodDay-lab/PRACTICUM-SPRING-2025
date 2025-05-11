#ifndef CPPLOX_ERRORSANDDEBUG_RUNTIMEERROR_H
#define CPPLOX_ERRORSANDDEBUG_RUNTIMEERROR_H
#pragma once

#include <exception>
#include <stdexcept>
#include <string>

#include "ErrorReporter.h"
#include "Token.h"

struct BreakException : std::exception {
    int line;
    explicit BreakException(int l) : line(l) {};
};

namespace cpplox::ErrorsAndDebug {

using Types::Token;

class RuntimeError : std::exception {};

auto reportRuntimeError(ErrorReporter& eReporter, const Token& token,
                        const std::string& message) -> RuntimeError;

}  // namespace cpplox::ErrorsAndDebug
#endif  // CPPLOX_ERRORSANDDEBUG_RUNTIMEERROR_H
