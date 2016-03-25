#ifndef SYNCSTACK_H_INCLUDED
#define SYNCSTACK_H_INCLUDED

#include <atomic>

template <typename E>
class LockFreeStack {
private:
    struct Node {
        E value;
        Node* next;

        Node(E newValue, Node* newNext = nullptr) {
            value = newValue;
            next = newNext;
        }
    };
    std::atomic<Node*> head;

public:
    LockFreeStack() {
        head.store(nullptr);
    }

    void push(E value) {
        Node* previousNode = head;
        Node* newNode = new Node(value);
        while (!head.compare_exchange_strong(previousNode, newNode));
        newNode->next = previousNode;
    }

    E pop() {
        Node* top = head;
        while (!head.compare_exchange_strong(top, top->next));
        E topValue = top->value;
        delete top;
        return topValue;
    }

    bool empty() {
        return head == nullptr;
    }
};

#endif // SYNCSTACK_H_INCLUDED
