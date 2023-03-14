
#ifndef SERIALHANDLER_H
#define SERIALHANDLER_H
#if 0
#include <QtCore>
#include <QtSerialPort/QSerialPort>
#include <QTextStream>
#include <QTimer>
#include <QByteArray>
#include <QObject>
#include <settingsdialog.h>

class SerialHandler : public QObject
{
    Q_OBJECT

private slots:
    void handleBytesWritten(qint64 bytes);
    void handleTimeout();

    void initSerialPort(SettingsDialog::Settings p);
    void closeSerialPort();
    void writeData(const QByteArray &data);
    void readData();
    void changeDesired(int x,int y);

    void handleError(QSerialPort::SerialPortError error);

public:
    QSerialPort    *serialPort;
    QByteArray      DataWrite;
    QTextStream     standardOutput;
    qint64          bytesWritten;
    QTimer          senderTimer;
public:
    SerialHandler(QSerialPort *serialPort, QObject *parent = 0);
    ~SerialHandler();
signals:
    void sendDataToConsole(QByteArray &data);

};
#endif
#endif // SERIALHANDLER_H
