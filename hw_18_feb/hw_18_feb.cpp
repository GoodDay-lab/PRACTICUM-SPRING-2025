#include <iostream>

using namespace std;

/*
1. Ошибка в том, что поля x, y имеют модификатор private, поэтому мы не можем к ним обращаться из блока main{}
    Иначе будет выведено:

    1011
*/

// class Cl {
// public:              // <=== Исправлено
//     int x, y;
// };

// int main() {
//     Cl c;
//     c.y = 10;
//     c.x = c.y++;
//     cout << c.x << c.y << endl;
//     return 0;
// }




/*
2. Ряд ошибок:
    1) Отсутствие конструктора при инициализации объекта `a` (без аргументов)
    2) Модификатор `explicit` в конструкторе не позволяет инициализировать объект `c`, так как запрещает неявное приведение типов
    Иначе выведет:

    11
*/

// struct A {
//     int n;
//     A() : n(0){};                   // <=== Fixed
//     A(double f) : n(f){};           // <===
// };

// int main() {
//     A a, b(1), c = 1.5;
//     cout << b.n << c.n << endl;
//     return 0;
// }




/*
3. Будет выведено:

    Constr (инициализация a(1))
    C_copy (инициализация первого аргумента функции f(a,a))
    C_copy (инициализация возвращаемого значения функции f(a,a))
    Destr (уничтожение первого аргумента)
    Destr (уничтожение возвращаемого значения)
    Destr (уничтожение изначального объекта `a`)
*/

// struct A {
//     int n;
//     A(int d) { n = d; cout << "Constr" << endl; };
//     A(A & a) { n = a.n; cout << "C_copy" << endl; };
//     ~A() { cout << "Destr" << endl; };
// };

// A& f(A & a, A & b) {            // Меняем типы возвращаемого значения и первого аргумента, чтобы не создавлись временные объекты
//     return a;
// }

// int main() {
//     A a(1);
//     a = f(a, a);
//     return 0;
// }




/*
1.11
    owl
    sheep 3
    sheep 7
    wolf
    horse 42
    sheep 1
    wolf
    wolf
    42 126
    wolf
    wolf
*/




/*
1.13
*/

// struct mystr {
//     int a, b;                 // В таком случае мы не можем проинициализировать структуру mystr конструктором `по умолчанию`
//     mystr(){};
// private:
//     mystr(mystr &){};
// };

// int i = sizeof(mystr);

// int f(mystr s) {
//     return 0;
// }

// int main()
// {
//     mystr t;
//     f(t);
//     cout << i << endl;
//     return 0;
// }



/*
1.14
*/

// struct smartstr {
//     int a = 0;
//     void *operator new[](size_t a) = delete;
// };

// int main() {
//     smartstr A;
//     //smartstr *B = new smartstr[20];
//     return A.a;
// }