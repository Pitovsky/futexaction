#include <mutex>
#include <condition_variable>
#include <limits>

template <typename C>
class PushPopWrapper {
private:
    typedef typename C::value_type value_type;
    C data;

public:
    void push(value_type element);
    value_type pop();
    size_t empty();
};

template<typename C>
size_t PushPopWrapper<C>::empty() {
    return data.empty();
}

///it is not work!
/*template<typename value_type>
void PushPopWrapper<std::queue<value_type>>::push(const value_type& element) {
    data.push(element);
}*/
template<typename C>
void PushPopWrapper<C>::push(value_type element) {
    data.push_back(element);
}

template<typename C>
typename PushPopWrapper<C>::value_type PushPopWrapper<C>::pop() {
    value_type element = data.back();
    data.pop_back();
    return element;
}

/*template<typename value_type>
class PushPopWrapper<std::queue<value_type>> {
private:
    std::queue<value_type> data;

public:
    void push(const value_type& element) {
        data->push(element);
    }
    void pop() {
        data->pop();
    }

    value_type& first() {
        return data->front();
    }

    size_t empty() {
        return data->empty();
    }
};*/

template <typename Container>
class CSyncContainer {
private:
    typedef typename Container::value_type value_type;

    PushPopWrapper<Container> data;

    std::condition_variable hasData;
    std::mutex mutex;


public:

    void push(const value_type& element) {
        std::unique_lock<std::mutex> security(mutex);
        data.push(element);
        hasData.notify_one();
    }

    value_type popOrWait() {
        std::unique_lock<std::mutex> security(mutex);
        while (data.empty()) {
            hasData.wait(security);
        }
        return data.pop();
    }

    static const value_type noElement = std::numeric_limits<value_type>::max(); //it is bad, but i am too lazy

    value_type popNoWait() { //pointer because null if no elements found
        std::unique_lock<std::mutex> security(mutex);
        if (data.empty()) {
            return noElement;
        }
        return data.pop();
    }

};
