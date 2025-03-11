#include "types.hpp"
#include <memory>
#include <stdexcept>
#include <iostream>
#include <cstring>
#include <random>

template <typename T>
class DynVector;

template <typename T>
class DynMatrix {
protected:
    inline static u32 counter = 0;

    const u32 id;
    T *mem;
    u32 size_1, size_2;
public:
    DynMatrix(u32 n, u32 m) : id(++counter), size_1(n), size_2(m) {
        mem = new T[n*m];
    };
    ~DynMatrix() {
        delete mem;
    };

    virtual u32 size() const {
        return size_1 * size_2;
    }

    void fill_random(u32 d) {
        for (u32 i = 0; i != size_1; i++) {
            for (u32 j = 0; j != size_2; j++) {
                mem[i * size_2 + j] = std::rand() % d;
            }
        }
    }

    virtual DynMatrix operator* (const DynMatrix& rhs) {
        if (this->size_2 != rhs.size_1)
            throw std::runtime_error(" Cannot mul A by B. ");
        
        DynMatrix ret(this->size_1, rhs.size_2);

        for (u32 i = 0; i != ret.size_1; i++) {
            for (u32 j = 0; j != ret.size_2; j++) {
                T S = 0;
                for (u32 k = 0; k != this->size_2; k++) {
                    S += (this->mem)[i * this->size_2 + k] * (rhs.mem)[k * rhs.size_2 + j];
                }
                ret.mem[i * ret.size_2 + j] = S;
            }
        }

        return ret;
    };

    virtual DynMatrix operator= (const DynMatrix& rhs) {
        DynMatrix ret(rhs.size_1, rhs.size_2);

        std::memcpy(ret.mem, rhs.mem, rhs.size());
        return ret;
    }

    DynMatrix tr() {
        DynMatrix ret(size_2, size_1);

        for (u32 i = 0; i != size_2; i++) {
            for (u32 j = 0; j != size_1; j++) {
                ret.mem[i * ret.size_2 + j] = this->mem[j * size_2 + i];
            }
        }

        return ret;
    }

    DynVector<T> operator[] (u32 x) {
        DynVector<T> ret(size_2);

        for (u32 i = 0; i != size_2; i++) {
            ret.mem[i] = this->mem[x * size_2 + i];
        };
        return ret;
    };

    template <typename U>
    friend std::ostream& operator<< (std::ostream& ct, const DynMatrix<U>& data);
};

template <typename T>
class DynVector : public DynMatrix<T> {
private:
    ;
public:
    DynVector(u32 n) : DynMatrix<T>(n, 1) {};
    ~DynVector() {};

    T operator[] (u32 i) {
        return this->mem[i];
    }
};


template <typename T>
std::ostream& operator<< (std::ostream& ct, const DynMatrix<T>& data) {
    ct << "[";
    for (u32 i = 0; i != data.size_1; i++) {
        for (u32 j = 0; j != data.size_2; j++) {
            ct << (data.mem)[i * data.size_2 + j];
            if (j != data.size_2 - 1)
                ct << ", ";
        }
        if (i != data.size_1 - 1)
            ct << '\n';
    }
    ct << "]\n";

    return ct;
}


int main() {
    DynVector<f32> v1(3);
    DynVector<f32> v2(3);
    DynMatrix<f32> m(4, 3);

    v1.fill_random(2);
    v2.fill_random(2);
    m.fill_random(4);

    DynMatrix<f32> v3 = v1.tr() * v2;

    std::cout << v1;
    std::cout << v2;
    std::cout << v3;
    std::cout << m;
    std::cout << m[1];
    std::cout << m[1][1];

    return 0;
}