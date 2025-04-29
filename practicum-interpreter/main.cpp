#include <bits/stdc++.h>

using namespace std;

#include "logging.cpp"
#include "scanner.cpp"
#include "expr.cpp"
#include "parser.cpp"

int runPrompt(int, const vector<string>&);
int runFile(int, const vector<string>&);

void run(const string&);

int main(int argc, char *argv[])    // langc [<script> | <filename>]
{
    vector<string> _argv;
    for (char **ptr = &argv[0]; *ptr != nullptr; ptr++)
        _argv.push_back(string(*ptr));

    int ret = 0;
    if (argc == 2)
        ret = runFile(argc, _argv);
    else
        ret = runPrompt(argc, _argv);

    return ret;
}


int runPrompt(int argc, const vector<string> &argv) {
    string inputLine;

    while (getline(cin, inputLine)) {
        run(inputLine);
        hadError = 0;
    }
    return 0;
}

int runFile(int argc, const vector<string> &argv) {
    ifstream file(argv[1]);
    if (!file.is_open()) {
        std::cerr << "Could not open file: " << argv[1] << std::endl;
        return 1;
    }

    stringstream inputStream;
    inputStream << file.rdbuf();
    string inputFile = inputStream.str();
    run(inputFile);

    if (hadError)
        return 2;
    return 0;
}

void run(const string &source) {
    initKeywords();

    Scanner scanner(source);
    vector<Token> tokens = scanner.scanTokens();

    Parser parser(tokens);
    list<Stmt*> statements = parser.parse2();

    ASTView viewer;
    cout << viewer.print(statements) << '\n';

    if (hadError)
        return;

    for (Token tok : tokens)
        cout << tok << '\n';
}

