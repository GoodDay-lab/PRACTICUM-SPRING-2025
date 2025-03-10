#include <iostream>

// Singletone

class SingletoneError {};

class Single {
private:
    static unsigned counter;
    static void _init() {
        if (!Single::counter) {
            Single::counter = 1;
        } else {
            throw SingletoneError();
        }
    };
    static void _delt() {
        if (Single::counter) {
            Single::counter = 0;
        } else {
            throw SingletoneError();
        }
    };

public:
    Single() {
        _init();
    }
    ~Single() {
        std::cout << Single::counter << std::endl;
        _delt();
    }
};

unsigned Single::counter = 0;

int main() {
    Single a;
}