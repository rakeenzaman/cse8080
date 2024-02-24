#include <iostream>

int main() {
    int number = 42;

    if (number < 10) {
        std::cout << 'A' << std::endl;
    } else if (number < 20) {
        std::cout << 'B' << std::endl;
    } else if (number < 30) {
        std::cout << 'C' << std::endl;
    } else {
        std::cout << 'D' << std::endl;
    }

    if (number % 2 == 0) {
        std::cout << 'E' << std::endl;
    } else {
        std::cout << 'F' << std::endl;
    }

    if (number % 3 == 0) {
        std::cout << 'G' << std::endl;
    } else {
        std::cout << 'H' << std::endl;
    }

    return 0;
}