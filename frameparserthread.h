
#ifndef FRAMEPARSERTHREAD_H
#define FRAMEPARSERTHREAD_H

#include <QThread>



class FrameParserThread : public QThread
{
public:
    explicit FrameParserThread(QObject *parent = nullptr);
private:
    void run() override;
};

#endif // FRAMEPARSERTHREAD_H
