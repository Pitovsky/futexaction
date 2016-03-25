#include <iostream>
#include <string>
#include <vector>
#include <deque>
#include <list>
#include <queue>
#include <stack>
#include <thread>
#include <time.h>
#include <algorithm>
#include "syncqueue.h"
#include "../testcontainer.h"

struct TS {
    char u;
    std::string v;
    TS(char nu = 'a', std::string nv = "undefined") {u = nu; v = nv;}
};

int main() {

    CSyncContainer<std::queue<TS>> testData;
    testData.push(TS('i', "pushed"));
    std::cout << testData.popOrWait().v << " ";
    std::cout << testData.popNoWait(nullptr).v << "\n";

    CSyncContainer<std::vector<int>> data;
    bool test;
    data.push(15);
    data.push(10);
    std::cout << data.popOrWait() << "\n";
    std::cout << data.popNoWait(&test) << ":";
    std::cout << test << "\n";
    std::cout << data.popNoWait(&test) << ":";
    std::cout << test << "\n";

    srand(time(NULL));
    for (size_t readerCount = 1; readerCount < 6; ++readerCount) {
        std::cout << "======================\nvector:\n";
        testSyncContainer<CSyncContainer<std::vector<int>>>::test(readerCount, 6 - readerCount, 10000); //for different count of pushers and printers
        std::cout << "======================\ndeque:\n";
        testSyncContainer<CSyncContainer<std::deque<int>>>::test(readerCount, 6 - readerCount, 10000);
        std::cout << "======================\nlist:\n";
        testSyncContainer<CSyncContainer<std::list<int>>>::test(readerCount, 6 - readerCount, 10000);
        std::cout << "======================\nqueue:\n";
        testSyncContainer<CSyncContainer<std::queue<int>>>::test(readerCount, 6 - readerCount, 10000);
        std::cout << "======================\nstack:\n";
        testSyncContainer<CSyncContainer<std::stack<int>>>::test(readerCount, 6 - readerCount, 10000);
    }
    return 0;
}
