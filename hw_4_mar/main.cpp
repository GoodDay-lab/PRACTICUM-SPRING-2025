/*
    Задание на 4 марта:
    1. Теория: слайды к занятию от 25.02.25
    2. Подготовиться к сдаче в МЗ №2 первого задания практикума.
    3. Задачи: по задачнику - 1.4, 2.1, 2.2 а),в), 2.3 а), в)
*/

#include <iostream>

using namespace std;

/*
 * 1.4
 */
/* 
class A {
    int field;
public:
    A(int d) : field(d) {};
    A& operator*= (const A& rhs) {
        field *= rhs.field;
        return *this;
    }

    string get() {
        return to_string(field) + " ";
    }
};

int main() {
    A a1(5), a2 = 3;
    a1 *= 10;
    a2 *= a1 *= 2;
    cout << a1.get() << a2.get() << endl;
}
*/


/*
 * 2.1
 */
/*
class Cls {
    int i;
public:
    Cls() { i = 1; }

    Cls& operator= (const class Cls& rhs) = delete;
};

void f(Cls* p, Cls* q) {
    *p = *q;
}
*/



/*
 * 2.2
 */

/*
void f(int, string) { cout << 1; };
void f(char, char = 0) { cout << 2; };

int main() {
    f(1);
    f('+', '+');
    f(2.3);
    f(3, "str");
}
*/

/*
void f(string a, int b) { cout << 1 << endl; };
void f(int a, string b) { cout << 2 << endl; };

int main() {
    f(0, 1);
    f(1, 1);
    f(0, "m");
    f("n", 0);
}
*/

/*
 * 2.3
 */

// 11211
// 11122

