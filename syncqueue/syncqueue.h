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

template <typename Container>
class CSyncContainer {
private:
    typedef typename Container::value_type value_type;

    PushPopWrapper<Container> data;

    std::condition_variable hasData;
    std::mutex mutex;


public:

    void push(value_type element); //note that element will be copied

    value_type popOrWait();

    value_type popNoWait(bool* success); //if success is not null, it will be changed on true ioi method return real popped element

    static const value_type noElement = std::numeric_limits<value_type>::max(); //is popNoWait was not success, it will be returned

};

template <typename Container>
void CSyncContainer<Container>::push(value_type element) {
    std::unique_lock<std::mutex> security(mutex);
    data.push(element);
    hasData.notify_one();
}

template <typename Container>
typename CSyncContainer<Container>::value_type CSyncContainer<Container>::popOrWait() {
    std::unique_lock<std::mutex> security(mutex);
    while (data.empty()) {
        hasData.wait(security);
    }
    return data.pop();
}

template <typename Container>
typename CSyncContainer<Container>::value_type CSyncContainer<Container>::popNoWait(bool* success) {
    std::unique_lock<std::mutex> security(mutex);
    if (data.empty()) {
        if (success != nullptr) {
            *success = false;
        }
        return noElement;
    }
    if (success != nullptr) {
        *success = true;
    }
    return data.pop();
}
