#include <iostream>

using namespace std;

// struct A {
//     int n;
// public:
//     A(int m) { n = m; };
// };

// void f(int i, char c) {
//     cout << '1' << endl;
// }

// void f(int, int) {
//     cout << '2' << endl;
// }

// void f(const char *p, char c) {
//     cout << '3' << endl;
// }

// void f(const char *p, A a) {
//     cout << '4' << endl;
// }

// int main() {
//     A a(1);
//     f(0, a); // 4th function
//     //f('*', a); // error, cause char !(=>) const char*
//     f(0, 'a'); // 1th function
// }

struct A {
    int n;
public:
    operator int() { return 1; }
};

void f(double, char) {
    cout << '1' << endl;
}

void f(double, int) {
    cout << '2' << endl;
}

void f(A, const char *) {
    cout << '3' << endl;
}

void f(int, const char*) {
    cout << '4' << endl;
}

int main() {
    A a;
    //f(a, 0);    // Error
    f(a, 'a');  // 
    //f('a', 0);  // Error
}