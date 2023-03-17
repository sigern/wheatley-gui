#ifndef QUEUE_H
#define QUEUE_H

#include <queue>
#include <deque>
#include <algorithm>

template <typename T, int maxSize>
class FixedQueue : public std::queue<T, std::deque<T>> {
public:
    void push(const T& value) {
        if (isFull()) {
            // QUEUE IS FULL
        }
        std::queue<T, std::deque<T>>::push(value);
    }

    void pushN(const T *array, size_t elementsToPush) {
        const size_t sizeLeft =  maxSize - this->size();
        if (elementsToPush <= sizeLeft) {
            this->c.insert(this->c.end(), array, array + elementsToPush);
        } else {
            this->c.insert(this->c.end(), array, array + sizeLeft);
        }

    }

    void popN(T *array, size_t elementsToPop) {
        size_t finalSizeToPop = elementsToPop;
        if (elementsToPop > this->size()) {
            finalSizeToPop = this->size();
        }
        std::copy_n(this->c.begin(), finalSizeToPop, array);
        this->c.erase(this->c.begin(), this->c.begin() + finalSizeToPop);
    }

    bool isFull() {
        return (this->size() == maxSize);
    }
};
class Queue
{

public:
    explicit Queue(Queue *parent = 0);
    ~Queue();
    int push(unsigned char new_element);
    int pop(unsigned char* old_element);

private:
    unsigned char* m_pData;
    int m_dataIn, m_dataOut;


};

#endif // QUEUE_H
