#include <iostream>
#include <vector>
#include "syncstack.h"

int main() {
    LockFreeStack<int> stack;
    stack.push(15);
    stack.push(10);
    std::cout << "p " << stack.pop() << "\n";
    stack.push(5);
    std::cout << "p " << stack.pop() << "\n";
    std::cout << "p " << stack.pop() << "\n";

    return 0;
}
