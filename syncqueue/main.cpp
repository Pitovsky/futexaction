#include <iostream>
#include <vector>
#include <queue>
#include "syncqueue.h"

template <typename Container>
struct Printer {
    void operator()(CSyncContainer<Container>& data, std::ostream& fout) {
        fout << data.popOrWait();
    }
};

template <typename Container>
struct Pusher {
    template <typename Iterator>
    void operator()(Iterator begin, Iterator end, CSyncContainer<Container>& data) {
        for (Iterator current = begin; current != end; ++current) {
            data.push(*current);
        }
    }
};

int main() {

    CSyncContainer<std::vector<int>> data;
    data.push(15);
    data.push(10);
    std::cout << data.popOrWait() << "\n";
    std::cout << data.popNoWait() << "\n";
    std::cout << data.popNoWait() << "\n";
}
