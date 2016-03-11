#include <iostream>
#include <atomic>
#include <thread>
#include <mutex>
#include <unistd.h>
#include <vector>
#include <chrono>

int global = 0;

class Futex {
private:
    std::atomic<int> ownerID;
public:
    Futex() {
        ownerID.store(-1);
    }

    void lock() {
        int wanted = -1;
        while (!ownerID.compare_exchange_strong(wanted, pthread_self())) {///pthread_self() is bad, but only way
            wanted = -1;
        }
    }

    void unlock() {
        int wanted = pthread_self();
        while (!ownerID.compare_exchange_strong(wanted, -1)) {
            wanted = pthread_self();
        }
    }

    Futex(const Futex& futex) = delete;
};

void incremerF(Futex& futex) {
    while (true) {
        futex.lock();
        if (global < 10000000) {
            ++global;
        } else {
            futex.unlock();
            break;
        }
        futex.unlock();
    }
}

void incremerM(std::mutex& mutex) {
    while (true) {
        mutex.lock();
        if (global < 10000000) {
            ++global;
        } else {
            mutex.unlock();
            break;
        }
        mutex.unlock();
    }
}

int main() {
    for (size_t threadsCount = 1; threadsCount < 6; ++threadsCount) {
        std::vector<std::thread> threads;
        Futex futex;
        std::mutex mutex;
        std::cout << "n = " << threadsCount << ":\n";
        global = 0;
        auto start = std::chrono::system_clock::now();
        for (size_t i = 0; i < threadsCount; ++i) {
            threads.push_back(std::thread(incremerF, std::ref(futex)));
        }
        for (size_t i = 0; i < threads.size(); ++i) {
            threads[i].join();
        }
        auto stop = std::chrono::system_clock::now();
        std::cout << " futex: " << std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count() << " ms\n";

        global = 0;
        threads.clear();
        start = std::chrono::system_clock::now();
        for (size_t i = 0; i < threadsCount; ++i) {
            threads.push_back(std::thread(incremerM, std::ref(mutex)));
        }
        for (size_t i = 0; i < threads.size(); ++i) {
            threads[i].join();
        }
        stop = std::chrono::system_clock::now();
        std::cout << " mutex: " << std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count() << " ms\n";
    }
    return 0;
}
