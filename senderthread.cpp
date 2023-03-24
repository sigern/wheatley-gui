
#include "senderthread.h"
#include <iostream>

SenderThread::SenderThread(QObject *parent)
    : QThread{parent}
{

}


void SenderThread::run()
{
    std::cout<<"Sender thread"<<std::endl;
}
