CXX_COMP = clang++
CXX_FLAGS = -std=c++20 -Wall -O1
TARGET = langc
SOURCE = DebugPrint.cpp Environment.cpp ErrorReporter.cpp Evaluator.cpp \
			InterpreterDriver.cpp Literal.cpp main.cpp NodeTypes.cpp \
			Objects.cpp Parser.cpp PrettyPrinter.cpp PrettyPrinterRPN.cpp \
			RuntimeError.cpp Scanner.cpp Token.cpp

build:
	$(CXX_COMP) $(CXX_FLAGS) $(SOURCE) -o $(TARGET)

clean:
	rm $(TARGET)
