
#include "frameparserthread.h"
#include <iostream>

FrameParserThread::FrameParserThread(QObject *parent)
    : QThread{parent}
{

}

void FrameParserThread::run()
{
    std::cout<<"Frameparser thread"<<std::endl;
}
