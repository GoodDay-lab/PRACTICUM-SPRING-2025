#ifndef CPPLOX_EVALUATOR_FUNCTION__H
#define CPPLOX_EVALUATOR_FUNCTION__H
#include <optional>
#pragma once

#include <map>
#include <memory>
#include <string>
#include <variant>

#include "NodeTypes.h"
#include "Token.h"
#include "Uncopyable.h"

namespace cpplox::Evaluator {

using LoxObject
    = std::variant<std::string, double, bool, std::nullptr_t>;

auto areEqual(const LoxObject& left, const LoxObject& right) -> bool;

auto getObjectString(const LoxObject& object) -> std::string;

auto isTrue(const LoxObject& object) -> bool;

class Environment;

class LoxBreak : public Types::Uncopyable {
 public:
   explicit LoxBreak();
};

}  // namespace cpplox::Evaluator

#endif  // CPPLOX_EVALUATOR_FUNCTION__H
