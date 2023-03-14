#include "serialhandler.h"
#if 0
SerialHandler::SerialHandler(QSerialPort *serialPort, QObject *parent)
    : QObject(parent)
    , serialPort(serialPort)
    , bytesWritten(0)
{
    connect(serialPort, SIGNAL(bytesWritten(qint64)), SLOT(handleBytesWritten(qint64));
    connect(serialPort, SIGNAL(error(QSerialPort::SerialPortError)), SLOT(handleError(QSerialPort::SerialPortError)));
    connect(senderTimer, SIGNAL(timeout()), SLOT(handleTimeout()));

    senderTimer.start(1000);
}

SerialHandler::~SerialHandler()
{

}

void SerialHandler::handleBytesWritten(qint64 bytes)
{
   // bytesWritten += bytes;
   // if (bytesWritten == DataWrite.size())
   // {
   //     bytesWritten = 0;
   //     qDebug()<<"Data successfully sent to port %1"<<endl;
   //     QCoreApplication::quit();
   // }
}

void SerialHandler::handleTimeout()
{
    qDebug()<<"Operation timed out for port %1, error: %2"<<endl;
    QCoreApplication::exit(1);
}

void SerialHandler::handleError(QSerialPort::SerialPortError serialPortError)
{
    //if (serialPortError == QSerialPort::ResourceError)
   // {
    //    QMessageBox::critical(this, tr("Critical Error"), serialPort->errorString());
    //    closeSerialPort();
    //}
}

void SerialHandler:: initSerialPort(SettingsDialog::Settings p)
{
    serialPort->setPortName(p.name);
    serialPort->setBaudRate(p.baudRate);
    serialPort->setDataBits(p.dataBits);
    serialPort->setParity(p.parity);
    serialPort->setStopBits(p.stopBits);
    serialPort->setFlowControl(p.flowControl);
}
#endif
