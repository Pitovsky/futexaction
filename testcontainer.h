#ifndef TESTCONTAINER_H_INCLUDED
#define TESTCONTAINER_H_INCLUDED

#include <vector>
#include <thread>
#include <string>
#include <algorithm>

/** test synchronized container in simple way - k threads write data into it, m threads read it, and then writen
  * and readed data are comparing on same elements containing.
  */
template <typename Container>
class testSyncContainer {
private:
    struct Printer {
        void operator() (Container& data, std::vector<int>& output, size_t mustPrint) {
            size_t printed = 0;
            while (printed < mustPrint) {
                output.push_back(data.popOrWait());
                ++printed;
            }
        }
        Printer() {}
    };

    struct Pusher {
        void operator() (const std::vector<int>& input, size_t begin, size_t end, Container& data) {
            for (size_t i = begin; i < end; ++i) {
                data.push(input[i]);
            }
        }
        Pusher() {}
    };

public:
    ///note, that methods use rand(), and you should do srand() before
    static bool test(size_t pushersCount, size_t printersCount, size_t dataPerThread, std::string containerName = "SyncCont") {
        Container data;
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
            pushers.push_back(std::thread(Pusher(), std::cref(tableData),
                    i * (dataSize / pushersCount), (i + 1) * (dataSize / pushersCount), std::ref(data)));
        }
        size_t sumOutputSize = 0;
        for (size_t i = 0; i < printersCount; ++i) {
            printers.push_back(std::thread(Printer(), std::ref(data), std::ref(outputs[i]), dataSize / printersCount));
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
        std::cout << "(data: " << dataSize << ") => " << pushersCount << "x pushers => (" << containerName << ") => "
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

    static bool differentThreadCountTest(size_t maxThreadCount, size_t dataPerThread, std::string containerName = "SyncCont") {
        for (size_t i = 1; i < maxThreadCount; ++i) {
            if (!test(i, maxThreadCount - i, dataPerThread, containerName)) {
                return false;
            }
        }
        return true;
    }

};

#endif // TESTCONTAINER_H_INCLUDED
