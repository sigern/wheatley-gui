#ifndef QUEUE_H
#define QUEUE_H

#define QUEUE_SIZE 20000

class Queue
{

public:
    explicit Queue(Queue *parent = 0);
    ~Queue();
    int  Push(unsigned char new_element);
    int  Pop(unsigned char* old_element);

private:
    unsigned char* Data;
    int DataIn, DataOut;


};

#endif // QUEUE_H
