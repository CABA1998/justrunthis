#include <iostream>
#include <string>

int main() {
    std::string name;
    std::cout << "What is your name? ";
    std::getline(std::cin, name);
    std::cout << "Hello, " << name << "! Nice to meet you." << std::endl;
    return 0;
}