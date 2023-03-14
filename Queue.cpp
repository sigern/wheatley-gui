#include "Queue.h"




Queue::Queue(Queue *parent)
{
    Data = new  unsigned char[QUEUE_SIZE];
    for(int i =0; i<QUEUE_SIZE; i++)
    {
        Data[i]=0;
    }
    DataIn = 0;
    DataOut = 0;
}

Queue::~Queue()
{
    delete[] Data;
}

int Queue::Push(unsigned char new_element)
{
  if(DataIn == (( DataOut - 1 + QUEUE_SIZE) % QUEUE_SIZE))
  {
    return -1; /* Queue Full*/
  }

  Data[DataIn] = new_element;
  DataIn = (DataIn + 1) % QUEUE_SIZE;

  return 0; // No errors
}

int Queue::Pop(unsigned char *old_element)
{
  if(DataIn == DataOut)
  {
    return -1; /* Queue Empty - nothing to get*/
  }
  *old_element = Data[DataOut];
  DataOut = (DataOut + 1) % QUEUE_SIZE;

  return 0; // No errors
}
