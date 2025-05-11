#include <iostream>

#include "InterpreterDriver.h"

// We are using SYSEXITS exit codes
auto main(int argc, char const *argv[]) -> int {
  if (argc > 2) {
    std::cout << "Usage: ./langc <script.lox> to execute a script or \
                  just ./langc to drop into a REPL"
              << std::endl;
    std::exit(64);
  }

  cpplox::InterpreterDriver interpreter;

  if (2 == argc) {
    return interpreter.runScript(argv[1]);
  }

  interpreter.runREPL();
  return 0;
}
