#include <iostream>
#include <cstdlib>

typedef __uint32_t u32;
typedef _Float32 f32;

template<typename T>
class Stack {
private:
    const static u32 MAX_SIZE = 256;
    T*    _memory;
    u32     _size;
    T*    _ptr;

public:
    Stack() {
        _memory = new T[MAX_SIZE];
        _size = 0;
        _ptr = _memory;
    };

    ~Stack() {
        delete _memory;
    }

    bool is_empty() {
        return _size == 0;
    }

    bool is_full() {
        return _size == MAX_SIZE;
    }

    bool push(const T& elem) {
        if (is_full())
            return false;
        
        if (_size != 0)
            _ptr++;
        *_ptr = elem;
        _size++;
        return true;
    }

    T pop() {
        if (is_empty())
            return 0;
        
        T ret = *_ptr;
        if (_size != 1)
            _ptr--;
        _size--;
        return ret;
    }

    Stack operator= (const Stack& oth) {
        for (u32 i = 0; i < MAX_SIZE; i++)
            *this->_memory = *oth->_memory;
        this->_size = oth->_size;
        this->_ptr = this->_memory + this->_size;

        return this;
    }

    // Stack& operator= (const Stack&& oth) {
    //     memcpy(this->_memory, oth->_memory, MAX_SIZE);
    //     this->_size = oth->_size;
    //     this->_ptr = this->_memory + this->_size;

    //     return *this;
    // }
};

int main()
{
    Stack<f32> S;
    S.push(3);
    S.push(4);
    S.push(5);
    S.push(1);
    S.push(42);
    S.push(1230.4);

    Stack<f32> D = S;

    while (!D.is_empty()) {
        std::cout << D.pop() << std::endl;
    }
}
