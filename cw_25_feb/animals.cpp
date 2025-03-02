#include <iostream>

class Animal {
private:
    std::string name;
public:
    Animal(const std::string &arg1) : name(arg1) {};
    ~Animal() {};

    const std::string &getName() {
        return name;
    }

    const std::string Voice() const {
        return "No animal";
    }
};

class Dog : public Animal {
public:
    Dog(const std::string &arg1) : Animal(arg1) {};

    const std::string Voice() const {
        return "Awf!";
    }
};

class Cat : public Animal {
public:
    Cat(const std::string &arg1) : Animal(arg1) {};

    const std::string Voice() const {
        return "Meow!";
    }
};

int main() {
    Dog berkley = Dog("Berkley");
    Cat handersson = Cat("Handersson");

    std::cout << "Hello, my name is " << berkley.getName() << ". " << berkley.Voice() << std::endl;

    return 0;
}