#include <mutex>
#include <condition_variable>

template <typename C>
class PushPopWrapper {
private:
    typedef typename C::value_type value_type;
    C data;

public:
    void push(value_type element) {
        data.push_back(element);
    }

    value_type pop() {
        value_type element = data.back();
        data.pop_back();
        return element;
    }

    size_t empty() {
        return data.empty();
    }
};

///Sorry for copypaste, but i had not found any working alternative ways
template <typename E>
class PushPopWrapper<std::queue<E>> {
private:
    typedef typename std::queue<E>::value_type value_type;
    std::queue<E> data;

public:
    void push(value_type element) {
        data.push(element);
    }

    value_type pop() {
        value_type element = data.front();
        data.pop();
        return element;
    }

    size_t empty() {
        return data.empty();
    }
};

template <typename E>
class PushPopWrapper<std::stack<E>> {
private:
    typedef typename std::stack<E>::value_type value_type;
    std::stack<E> data;

public:
    void push(value_type element) {
        data.push(element);
    }

    value_type pop() {
        value_type element = data.top();
        data.pop();
        return element;
    }

    size_t empty() {
        return data.empty();
    }
};



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

    static const value_type noElement; //standart-constructed value_type. If popNoWait was not success, it will be returned

};

template <typename Container>
const typename CSyncContainer<Container>::value_type CSyncContainer<Container>::noElement = value_type();

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
