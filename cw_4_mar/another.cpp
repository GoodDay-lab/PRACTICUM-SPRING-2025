#include <iostream>

using namespace std;

class L {
private:
    int a;
public:
    L(int b) : a(b) { cout << "THIS\n"; }
    int get() { return a; }
};

class L2 {
private:
    int b;
public:
    L2(int c) : b(c) { cout << "THIS2\n"; }
    int getb() { return b; }
};

class A : virtual public L {
public:
    A(int z) : L(z-1) { cout << "A\n"; };
};

class B : virtual public L {
public:
    B(int z) : L(z+1) { cout << "B\n"; };
};

class C : public A, public B, public L2 {
public:
    C(int d) : L(d), A(d), B(d), L2(d) {};
};

class shape {
public:
    virtual double area() = 0;
};

class rect : public shape {
    double h, w;
public:
    rect(double a, double b) : h(a), w(b) {};
    double area() { return h * w; }
};

class circ : public shape {
    double r;
public:
    circ(double r) : r(r) {};
    double area() { return 3.141 * r * r; }
};

int main() {
    shape* arr[3];
    arr[0] = new rect(1.5, 4.0);
    arr[1] = new circ(0.4);
    arr[2] = new circ(1.6);

    for (int i = 0; i != 3; i++) {
        cout << arr[i]->area() << endl;
    }
}