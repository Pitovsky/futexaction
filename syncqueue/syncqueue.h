#include <mutex>
#include <condition_variable>

template <typename Container>
class CSyncContainer {
private:
    typedef typename Container::value_type value_type;
    Container data;
    std::condition_variable hasData;
    std::mutex mutex;


    value_type& hardPop() {
        value_type& value = data.back(); //unfortunately, in one case we must return vector.back(), in other queue.front()
        if (/*pop exists*/ true) {
            value = data.front();
            data.pop();
        } else {
            data.pop_back(); //i want to do pop_front, but in task...
        }
        return value;
    }


public:

    void push(const value_type& element) {
        std::unique_lock<std::mutex> security(mutex);
        if (/*push exists*/ true) {
            data.push(element);
        } else {
            data.push_back(element);
        }
        hasData.notify_one();
    }

    value_type& popOrWait() {
        std::unique_lock<std::mutex> security(mutex);
        while (data.empty()) {
            hasData.wait(security);
        }
        return hardPop();
    }

    value_type* popNoWait() { //* because null if no elements found
        std::unique_lock<std::mutex> security(mutex);
        if (data.empty()) {
            return nullptr;
        }
        return *hardPop();
    }

    void suicide() {
    }
};
