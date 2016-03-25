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

template <typename Container>
void printer(CSyncContainer<Container>& data, std::vector<int>& output, size_t mustPrint) {
    size_t printed = 0;
    while (printed < mustPrint) {
        output.push_back(data.popOrWait());
        ++printed;
    }
}

template <typename Container>
void pusher(const std::vector<int>& input, size_t begin, size_t end, CSyncContainer<Container>& data) {
    for (size_t i = begin; i < end; ++i) {
        data.push(input[i]);
    }
}

template <typename Container>
bool testCSyncContainer(size_t pushersCount, size_t printersCount, size_t dataPerThread) {
    CSyncContainer<Container> data;
    size_t dataSize = dataPerThread * pushersCount * printersCount;
    std::vector<int> tableData(dataSize);
    for (size_t i = 0; i < tableData.size(); ++i) {
        tableData[i] = rand() % 1000;
    }
    std::sort(tableData.begin(), tableData.end());

    std::vector<std::thread> pushers;
    std::vector<std::thread> printers;
    std::vector<std::vector<int>> outputs(printersCount);

    for (size_t i = 0; i < pushersCount; ++i) {
        pushers.push_back(std::thread(pusher<Container>, std::cref(tableData),
                i * (dataSize / pushersCount), (i + 1) * (dataSize / pushersCount), std::ref(data)));
    }
    size_t sumOutputSize = 0;
    std::mutex sumOutputMutex;
    for (size_t i = 0; i < printersCount; ++i) {
        printers.push_back(std::thread(printer<Container>, std::ref(data), std::ref(outputs[i]), dataSize / printersCount));
    }

    for (size_t i = 0; i < pushersCount; ++i) {
        pushers[i].join();
    }
    for (size_t i = 0; i < printersCount; ++i) {
        printers[i].join();
    }

    std::vector<int> finalData;
    for (size_t i = 0; i < printersCount; ++i) {
        /*std::cout << "[";
        for (size_t j = 0; j < outputs[i].size(); ++j) {
            std::cout << outputs[i][j] << ", ";
        }
        std::cout << "]\n";*/
        finalData.insert(finalData.end(), outputs[i].begin(), outputs[i].end());
    }
    std::sort(finalData.begin(), finalData.end());
    std::cout << "(data: " << dataSize << ") => " << pushersCount << "x pushers => (CSyncContainer) => "
            << printersCount << "x printers: ";
    if (finalData == tableData) {
        std::cout << "OK\n";
    } else {
        std::cout << "FAIL: expected\n[" << tableData[0];
        for (size_t i = 1; i < tableData.size(); ++i) {
            std::cout << ", " << tableData[i];
        }
        std::cout << "]\n, but was\n[" << finalData[0];
        for (size_t i = 1; i < finalData.size(); ++i) {
            std::cout << ", " << finalData[i];
        }
        std::cout << "]\n";
        return false;
    }
    return true;
}

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
        testCSyncContainer<std::vector<int>>(readerCount, 6 - readerCount, 10000); //for different count of pushers and printers
        std::cout << "======================\ndeque:\n";
        testCSyncContainer<std::deque<int>>(readerCount, 6 - readerCount, 10000);
        std::cout << "======================\nlist:\n";
        testCSyncContainer<std::list<int>>(readerCount, 6 - readerCount, 10000);
        std::cout << "======================\nqueue:\n";
        testCSyncContainer<std::queue<int>>(readerCount, 6 - readerCount, 10000);
        std::cout << "======================\nstack:\n";
        testCSyncContainer<std::stack<int>>(readerCount, 6 - readerCount, 10000);
    }
    return 0;
}
