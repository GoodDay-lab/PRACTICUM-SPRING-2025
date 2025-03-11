#include <iostream>
#include <stdexcept>
#include <memory>
#include <bits/stdc++.h>

typedef double f32;
typedef long double f64;

template <typename T>
class Matrix {
private:
    static size_t obj_counter;

    T* buffer;
    size_t height, width;

public:
    Matrix(size_t h, size_t w) : height(h), width(w) {
        Matrix<T>::obj_counter++;
        std::cout << "Created matrix (" << h << ", " << w << "): total " << Matrix<T>::obj_counter << std::endl;
    
        buffer = new T[h*w];
    }
    Matrix(std::pair<size_t, size_t> size) : Matrix(size.first, size.second) {};

    ~Matrix() {
        Matrix<T>::obj_counter--;
        std::cout << "Destroyed matrix: total " << Matrix<T>::obj_counter << std::endl;
    
        delete buffer;
    }

    void fill(size_t d = 10) {
        for (size_t i = 0; i != height; i++) {
            for (size_t j = 0; j != width; j++) {
                buffer[i * width + j] = std::rand() % d;
            }
        }
    }

    std::pair<size_t, size_t> getSize() const {
        std::pair<size_t, size_t> ret = std::make_pair(height, width);
        return ret;
    }

    Matrix<T> transpose() {
        std::pair<size_t, size_t> trSize = this->getSize();
        Matrix<T> ret(trSize.second, trSize.first);

        for (size_t i = 0; i != height; i++) {
            for (size_t j = 0; j != width; j++) {
                ret.buffer[j * height + i] = this->buffer[i * width + j];
            }
        }

        return ret;
    }

    Matrix<T> operator* (const Matrix<T>& rhs) {
        std::pair<size_t, size_t> fSize = this->getSize(), sSize = rhs.getSize();
        Matrix<T> ret(fSize.first, sSize.second);

        for (size_t i = 0; i != fSize.first; i++) {
            for (size_t j = 0; j != sSize.second; j++) {
                T summ = 0;
                for (size_t k = 0; k != fSize.second; k++) {
                    summ += this->buffer[i * fSize.second + k] * this->buffer[k * sSize.second + j];
                }

                ret.buffer[i * sSize.second + j] = summ;
            }
        }

        return ret;
    }
};

template <typename T>
size_t Matrix<T>::obj_counter = 0;


int main()
{
    Matrix<f32> _this(4, 3);
    Matrix<f32> _this2(3, 1);
    Matrix<f32> _this3 = _this.transpose();

    _this.fill(4);

    std::cout << _this3.getSize().first << std::endl;
    std::cout << (_this * _this2).getSize().first << std::endl;
    return 0;
}