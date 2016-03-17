#include <mutex>
#include <condition_variable>

template <typename Container>
class CSyncContainer {
private:
    typedef typename Container::value_type value_type;

    Container data;

    class PushPopWrapper {
    private:
        Container data;

    public:
        void push(const value_type& element) {
            data.push_back(element);
        }
        void pop() {
            data.pop_back();
        }
    };
    PushPopWrapper* pushPopData = nullptr;

    std::condition_variable hasData;
    std::mutex mutex;

    value_type& hardPop() {
        if (/*pop exists*/ true) {
            value_type& value = data.front();
            data.pop();
            return value;
        } else {
            value_type& value = data.back();
           // data.pop_back(); //i want to do pop_front, but in task...
            return value;
        }
    }

    class hasPushPopMethods {
    private:
        typedef long yes;
        typedef char no; //only different-sized types

        ///dirty hack: if method exist, it will be more priority than '...'
        template<typename C, void (C::*)() = &C::push> static yes test( C* );//now only push test, but for STL it is enough
        template <typename C> static no test(...);

    public:
        static const int value = sizeof(test<Container>(nullptr));
        //bool value = (sizeof(test<Container>(0)) == sizeof(yes));
    };

public:
    CSyncContainer() {
        std::cout << hasPushPopMethods::value << "\n";
    }

    void push(const value_type& element) {
        std::unique_lock<std::mutex> security(mutex);
        if (/*push exists*/ true) {
            data.push(element);
            std::cout << "pushed" << element << "\n";
        } else {
          //  data.push_back(element);
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

    value_type* popNoWait() { //pointer because null if no elements found
        std::unique_lock<std::mutex> security(mutex);
        if (data.empty()) {
            return nullptr;
        }
        return &hardPop();
    }

    void suicide() {
    }
};
