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

    bool popInto(E& element) {
        Node* top = head;
        do {
            if (top == nullptr) {
                return false;
            }
        } while (!head.compare_exchange_strong(top, top->next));
        element = top->value;
        delete top;
        return true;
    }

    void pop() {
        E temp;
        popInto(temp);
    }

    bool empty() {
        return head == nullptr;
    }
};

#endif // SYNCSTACK_H_INCLUDED
