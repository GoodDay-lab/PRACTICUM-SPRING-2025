#ifndef _LOGGING_CPP
#define _LOGGING_CPP

#include <bits/stdc++.h>

using namespace std;

static int hadError;

void report(int _line, string _where, string _msg) {
    cerr << "[at " << _line << "] Error" << _where << ": " << _msg << '\n';
    hadError = true;
}

void error(int _line, string _msg) {
    report(_line, "", _msg);
}


#endif
