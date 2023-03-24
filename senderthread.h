
#ifndef SENDERTHREAD_H
#define SENDERTHREAD_H

#include <QThread>



class SenderThread : public QThread
{
public:
    explicit SenderThread(QObject *parent = nullptr);

private:
    void run() override;
};

#endif // SENDERTHREAD_H
