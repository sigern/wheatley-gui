#include "Queue.h"

Queue::Queue(Queue *parent)
{
    m_pData = new unsigned char[QUEUE_SIZE];
    for(int i =0; i < QUEUE_SIZE; i++){
        m_pData[i] = 0;
    }
    m_dataIn = 0;
    m_dataOut = 0;
}

Queue::~Queue()
{
    delete[] m_pData;
}

int Queue::push(unsigned char new_element)
{
    if(m_dataIn == (( m_dataOut - 1 + QUEUE_SIZE) % QUEUE_SIZE)) {
        return -1; /* Queue Full*/
    }

    m_pData[m_dataIn] = new_element;
    m_dataIn = (m_dataIn + 1) % QUEUE_SIZE;

  return 0; // No errors
}

int Queue::pop(unsigned char *old_element)
{
    if(m_dataIn == m_dataOut) {
        return -1; /* Queue Empty - nothing to get*/
    }
    *old_element = m_pData[m_dataOut];
    m_dataOut = (m_dataOut + 1) % QUEUE_SIZE;

    return 0; // No errors
}
