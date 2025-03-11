#include <iostream>

class Logger {
private:
    static int counter;
    const int id;

public:
    Logger(): id(++counter) {
        std::cout << "Logger(): " << id << "\n";
    }

    Logger(const Logger& other): id(++counter) {
        std::cout << "Logger(const Logger&): " << id << " " << other.id << "\n";
    }

    Logger(Logger&& other): id(++counter) {
        std::cout << "Logger(Logger&&): " << id << " " << other.id << "\n";
    }

    Logger& operator = (const Logger& other) {
        std::cout << "Logger& operator = (const Logger&): " << id << " " << other.id << "\n";
        return *this;
    }

    Logger& operator = (Logger&& other) {
        std::cout << "Logger& operator = (Logger&&): " << id << " " << other.id << "\n";
        return *this;
    }

    ~Logger() {
        std::cout << "~Logger(): " << id << "\n";
    }
};

int Logger::counter = 0;


#include <vector>
#include <iostream>

int main()
{
    int n;
    std::cin >> n;
    std::vector<Logger> v(n);
}