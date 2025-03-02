#include <iostream>
#include <string>

static std::string mailboxes[] = {"gmail.com", "yandex.ru", "ya.ru", "gse.cs.msu.ru", "rambler.ru", "msu.ru", "mipt.ru", "hse.ru"};

class EmailAddr {
private:
    std::string name;
    std::string mailbox;

public:
    EmailAddr(std::string name, std::string mail) : name(name), mailbox(mail) {
        std::cout << "Mail " << name << '@' << mail << " created!" << std::endl;
    };
    ~EmailAddr() {
        std::cout << "Mail " << name << '@' << mailbox << " deleted..." << std::endl;
    };

    void setName(std::string name) {
        if (1) {
            this->name = name;
        };
    };
    void setMailbox(std::string mail) {
        if (1) {
            this->mailbox = mail;
        };
    };

    std::string getAddress() { return name + "@" + mailbox; };
    std::string getName() { return name; };
    std::string getMailbox() { return mailbox; };
};

int main() {
    EmailAddr MSU = EmailAddr("msu-university", "msu.ru");
    EmailAddr MIPT = EmailAddr("mipt-institute", "mipt.ru");
    EmailAddr HSE = EmailAddr("hse-university", "hse.ru");

    std::cout << MSU.getAddress() << std::endl;
    std::cout << MIPT.getAddress() << std::endl;

    MSU.setName("new-msu-university");

    std::cout << MSU.getAddress() << std::endl;
}