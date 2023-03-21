/****************************************************************************
**
** Copyright (C) 2012 Denis Shienkov <denis.shienkov@gmail.com>
** Copyright (C) 2012 Laszlo Papp <lpapp@kde.org>
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtSerialPort module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL21$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia. For licensing terms and
** conditions see http://qt.digia.com/licensing. For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights. These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "mainwindow.h"
#include "Queue.h"
#include "qobjectdefs.h"
#include "ui_mainwindow.h"
#include "console.h"
#include "gamepaddisplay.h"
#include "settingsdialog.h"

#include <iostream>
#include <string>
#include <QMessageBox>
#include <QQuickItem>
#include <QtSerialPort/QSerialPort>
#include <QFile>

//! [0]
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
//! [0]
    //this->setFixedSize(810,610);
    ui->setupUi(this);
    m_console = new Console;
    m_console->setEnabled(false);    
    //setCentralWidget(console);
    QVBoxLayout *vbox = new QVBoxLayout;
    vbox->addWidget(m_console);
    ui->console_group->setLayout(vbox);
//    ui->console_group->setFixedHeight(300);
//    ui->console_group->setFixedWidth(480);

//    QUrl source("qrc:/Joystick.qml");
//    ui->joystickQuickItemLayout->addWidget(m_quickWidget);
//    ui->joystick_group->setFixedHeight(1000);
//    ui->joystick_group->setFixedWidth(1000);
//    m_quickWidget ->resize(100,100);
//      m_quickWidget->setResizeMode(QQuickWidget::SizeRootObjectToView );
//      m_quickWidget->setSource(source);


   // rootObject = dynamic_cast<QObject*>(m_quickWidget->rootObject());
    QObject::connect(ui->joystickQuickItem->rootObject(), SIGNAL(joystickChanged(int, int)), this, SLOT(changeDesired(int, int)));


//    ui->p_value->setText(QString::number(0,'f',1));
//    ui->i_value->setText(QString::number(0,'f',1));
//    ui->d_value->setText(QString::number(0,'f',1));

   // setCentralWidget(centralWidget);
    m_serialPort = new QSerialPort(this);
    m_settings = new SettingsDialog;
    m_gamepadDisplay = new GamepadDisplay;
    m_gamepadController = new SimpleXbox360Controller(0);
    m_gamepadController->startAutoPolling(20);

    stickObject = ui->joystickQuickItem->rootObject()->findChild<QObject*>("stick");
    joyStickObject = ui->joystickQuickItem->rootObject()->findChild<QObject*>("joyStick");

    ui->lipol_val->setText(QString::number(0,'f',2)+" V");
    ui->roll_val->setText(QString::number(0,'f',1));
    ui->tilt_val->setText(QString::number(0,'f',1));
    ui->velocity_val->setText(QString::number(0,'f',1));
    ui->spinBox_roll->setValue(SERVO_INPUT_ZERO);
    ui->spinBox_tilt->setValue(SERVO_INPUT_ZERO);
    ui->sliderRoll->setValue(SERVO_INPUT_ZERO);
    ui->sliderTilt->setValue(SERVO_INPUT_ZERO);
    ui->actionConnect->setEnabled(true);
    ui->actionDisconnect->setEnabled(false);
    ui->actionQuit->setEnabled(true);
    ui->actionConfigure->setEnabled(true);
    ui->xboxButton->setEnabled(false);

    setControlsEnabled(false);
    connectSignals();
}

MainWindow::~MainWindow()
{
    delete m_settings;
    delete m_gamepadDisplay;
    delete m_gamepadController;
    delete m_serialPort;
    delete m_console;
    delete ui;
}

void MainWindow::openSerialPort()
{
    SettingsDialog::Settings p = m_settings->settings();
    m_serialPort->setPortName(p.name);
    m_serialPort->setBaudRate(p.baudRate);
    m_serialPort->setDataBits(p.dataBits);
    m_serialPort->setParity(p.parity);
    m_serialPort->setStopBits(p.stopBits);
    m_serialPort->setFlowControl(p.flowControl);
    if (m_serialPort->open(QIODevice::ReadWrite)) {
        m_console->setEnabled(true);
        m_console->setLocalEchoEnabled(p.localEchoEnabled);
        ui->actionConnect->setEnabled(false);
        ui->actionConfigure->setEnabled(false);
        ui->actionDisconnect->setEnabled(true);
        ui->status_val->setText("<font color='green'>CONNECTED</font>");
        ui->statusBar->showMessage(tr("Connected to %1 : %2, %3, %4, %5, %6")
                                   .arg(p.name).arg(p.stringBaudRate).arg(p.stringDataBits)
                                   .arg(p.stringParity).arg(p.stringStopBits).arg(p.stringFlowControl));
        m_console->putData("*** Connection successfully established ***\n");

        setControlsEnabled(true);
        //senderTimer->start(1);
        m_receiverTimer.start(100);
        std::cout<<"Startimg timers"<<std::endl;
    }
    else
    {
        QMessageBox::critical(this, tr("Error"), m_serialPort->errorString());

        ui->statusBar->showMessage(tr("Open error"));
        m_console->putData("*** Connection encountered an error ***\n");

    }
}

void MainWindow::closeSerialPort()
{
    m_serialPort->close();
    m_console->setEnabled(false);

    ui->actionConnect->setEnabled(true);
    ui->actionDisconnect->setEnabled(false);
    ui->actionConfigure->setEnabled(true);
    setControlsEnabled(false);
    ui->statusBar->showMessage(tr("Disconnected"));
    ui->status_val->setText("<font color='red'>DISCONNECTED</font>");

    m_console->putData("*** Connection aborted by user ***\n");

    m_senderTimer.stop();
    m_receiverTimer.stop();
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("About Wheatley - PC Application"),
                       QString("The <b>Wheatley - PC Application</b> is a software that allows user<br>"
                               "to wirelessly connect and control the:<br>"
                               "<center> <i>Mobile Spherical Robot - Wheatley</i></center><br>"
                               "It was created as a part of the engineering thesis project by:<br>"
                               "<center><i>Kacper Łanda<br>"
                               "2011-2016 student<br>"
                               "Automatic Control and Robotics<br>"
                               "AGH University of Science and Technology</i></center><br><br>"
                               "<center>January 2015</center>"));
}

void MainWindow::writeData(const QByteArray &data)
{
    m_serialPort->write(data);
}

void MainWindow::readData()
{
//    static const unsigned char start_frame = 0xFF;
//    static const unsigned char end_frame = 0xFE;
//    static const unsigned char separate_frame = 0xFA;
//    static const unsigned char joystick_frame = 0xFD;
//    static const unsigned char pid_frame = 0xFC;
//    static const unsigned char control_frame =0xFB;

    const QByteArray incomingData = m_serialPort->readAll();
    const char* pIncomingCharData = incomingData.constData();
    if (!incomingData.isEmpty()) {
        std::stringstream ss;
        ss << "Read " << incomingData.size() <<" bytes:";
        for(int i = 0; i < incomingData.size(); i++) {
            ss <<std::hex<< " " << int(*(pIncomingCharData + i) & 0xFF);
        }
        ss << "\n";
        m_console->putData(ss.str().c_str());
        m_receiverQueue.pushN(pIncomingCharData, incomingData.size());
    }

    if (m_receiverQueue.isFull()) {
       m_console->putData("RECEIVE QUEUE FULL\n");
    }
    /*
        frameHandler->Push(start_frame);
        frameHandler->Push(sensor_frame);
        frameHandler->Push(separate_frame);
        frameHandler->Push(0);
        frameHandler->Push(1);
        frameHandler->Push(0);
        frameHandler->Push(1);
        frameHandler->Push(0);
        frameHandler->Push(1);
        frameHandler->Push(separate_frame);
        frameHandler->Push(0);
        frameHandler->Push(1);
        frameHandler->Push(0);
        frameHandler->Push(1);
        frameHandler->Push(0);
        frameHandler->Push(1);
        frameHandler->Push(separate_frame);
        frameHandler->Push(0);
        frameHandler->Push(1);
        frameHandler->Push(0);
        frameHandler->Push(1);
        frameHandler->Push(0);
        frameHandler->Push(1);
        frameHandler->Push(end_frame);
        */

    //console->putData(data);
    //qDebug()<<"Length:"<<data.toStdString().length()<<endl;
    /*
    for(unsigned int z=0; z<data.toStdString().length(); z++)
    {
        switch((unsigned char)(data.data()[z]))
        {
            case start_frame:
                if(command == NONE)
                {
                    command = CHECK;
                    //qDebug()<<"NONE->CHECK"<<endl;
                }
               break;
            case control_frame:
                if(command == CHECK)
                {
                    command = CONTROL;
                    //qDebug()<<"CHECK->CONTROL"<<endl;
                }
                break;
            case pid_frame:
                if(command == CHECK)
                {
                    command = PID;
                    //qDebug()<<"CHECK->PID"<<endl;
                }
                break;
            case separate_frame:
                //qDebug()<<"CONTROL->NONE"<<endl;
                break;
            case end_frame:
                if(((command == CONTROL) || (command == PID)) && (iter == 12))
                {
                    command = NONE;
                    //qDebug()<<"CONTROL->NONE"<<endl;
                    iter = 0;
                    memcpy((void*)&RobotTransfer_Struct->roll_servo,(void*)&temp_RobotTransfer[0],12);
                    //memcpy((void*)&RobotTransfer_Struct->tilt_servo,(void*)&temp_RobotTransfer[2],2);
                    //memcpy((void*)&RobotTransfer_Struct->lipol_vol,(void*)&temp_RobotTransfer[4],2);
                   // memcpy((void*)&RobotTransfer_Struct->x_current,(void*)&temp_RobotTransfer[6],2);
                   // memcpy((void*)&RobotTransfer_Struct->y_current,(void*)&temp_RobotTransfer[8],2);
                   // memcpy((void*)&RobotTransfer_Struct->velocity,(void*)&temp_RobotTransfer[10],2);

                    ui->roll_val->setText(QString::number(RobotTransfer_Struct->roll_servo));
                    ui->tilt_val->setText(QString::number(RobotTransfer_Struct->tilt_servo));
                    //ui->lipol_val->setText(QString::number(RobotTransfer_Struct->lipol_vol));
                    ui->x_current_val->setText(QString::number((double)RobotTransfer_Struct->x_current-101.0,'f',1));
                    ui->y_current_val->setText(QString::number((double)RobotTransfer_Struct->y_current-101.0,'f',1));
                    if(RobotTransfer_Struct->velocity>3000)
                         ui->velocity_val->setText(QString::number((double)(RobotTransfer_Struct->velocity-3000)/1000.0,'f',3));
                    else
                         ui->velocity_val->setText(QString::number(-(double)(RobotTransfer_Struct->velocity)/1000.0,'f',3));

            //////////////////////////////////////////
                    temp_PCTransfer[0] =  PCTransfer_Struct->x_desired&0x00FF;
                    qDebug()<<"reveived tilt = "<<RobotTransfer_Struct->tilt_servo<<endl;
                    qDebug()<<"send     x = "<<temp_PCTransfer[0]<<endl;
                    temp_PCTransfer[1] =  0;//(PCTransfer_Struct->x_desired&0xFF00)>>8;

                    temp_PCTransfer[2] =  PCTransfer_Struct->y_desired&0x00FF;
                    qDebug()<<"received roll = "<<RobotTransfer_Struct->roll_servo<<endl;
                    qDebug()<<"send     y = "<<temp_PCTransfer[2]<<endl;
                    temp_PCTransfer[3] =  0;//(PCTransfer_Struct->y_desired&0xFF00)>>8;
                    //qDebug()<<"[0]-"<<(unsigned int)(temp_PCTransfer[0]&0x00FF)<<" [2]-"<<(unsigned int)(temp_PCTransfer[2]&0x00FF)<<endl;
                    serial->write((char*)&start_frame);
                    serial->write((char*)&joystick_frame);
                   // serial->write(&separate_frame);
                    serial->write((char*)&temp_PCTransfer[0]);
                    //serial->write(&separate_frame);
                    serial->write((char*)&temp_PCTransfer[2]);
                    serial->write((char*)&end_frame);
            //////////////////////////////////////////
                }
                break;
            default:
                if(command == CONTROL && iter<12)
                {
                    temp_RobotTransfer[iter] = data.data()[z];
                    iter++;
                    //qDebug()<<"data["<<z<<"]="<<data.data()[z]<<endl;
                    //qDebug()<<"Iter:"<<iter<<endl;
                }
                break;
        }

    }
    */
    //qDebug()<<"PC - roll:"<<RobotTransfer_Struct->roll_servo<<", tilt:"<<RobotTransfer_Struct->tilt_servo<<", lipol:"<<RobotTransfer_Struct->lipol_vol<<", x_cur:"<<RobotTransfer_Struct->x_current<<", y_cur:"<<RobotTransfer_Struct->y_current<<", v:"<<RobotTransfer_Struct->velocity<<endl;
    //qDebug()<<"     x_des:"<<PCTransfer_Struct->x_desired<<", y_des:"<<PCTransfer_Struct->y_desired<<endl;
    //console->putData(data);

}

void MainWindow::updateUiControlValues()
{
    ui->lipol_val->setText(QString::number(m_wheatley.lipol_vol, 'f', 2)+" V");
    ui->roll_val->setText(QString::number(m_wheatley.roll_servo, 'i', 1));
    ui->tilt_val->setText(QString::number(m_wheatley.roll_servo, 'i', 1));
    ui->velocity_val->setText(QString::number(m_wheatley.velocity, 'f', 1));
}

void MainWindow::changeDesired(int tilt, int roll)
{
//    ui->x_desired_val->setText(QString::number(x,'f',1));
//    ui->y_desired_val->setText(QString::number((-y+200),'f',1));
    int index = ui->controller_comboBox->currentIndex();
    if(index == 0)
    {
        m_wheatley.tilt_servo = tilt;
        m_wheatley.roll_servo = roll;
        ui->spinBox_tilt->setValue(int(tilt));
        ui->spinBox_roll->setValue(int(roll));
    }



    /*
    Wheatley->x_angle = ((double(x)-50.0)*2+100);

    if(Wheatley->x_angle==0)
        Wheatley->x_angle=2;
    if(Wheatley->x_angle==48)
        Wheatley->x_angle=50;

    Wheatley->y_angle = (-(double(y)-50.0)*2+100);

    if(Wheatley->y_angle==2 || Wheatley->y_angle==6)
        Wheatley->y_angle=4;
    if(Wheatley->y_angle==8)
        Wheatley->y_angle=9;
    if(Wheatley->y_angle==0)
        Wheatley->y_angle=4;
    if(Wheatley->x_angle==100)
        Wheatley->x_angle=101;
    if(Wheatley->y_angle==100)
        Wheatley->y_angle=101;

    PCTransfer_Struct->x_desired = Wheatley->x_angle;
    PCTransfer_Struct->y_desired = Wheatley->y_angle;
    if(Wheatley->x_angle>=0 && Wheatley->x_angle<10.0)
        ui->x_desired_val->setText("  "+QString::number(Wheatley->x_angle-101.0,'f',1));
    else if ((Wheatley->x_angle<0 && Wheatley->x_angle>-10.0) || Wheatley->x_angle>=10.0)
        ui->x_desired_val->setText(" "+QString::number(Wheatley->x_angle-101.0,'f',1));
    else
        ui->x_desired_val->setText(QString::number(Wheatley->x_angle-101.0,'f',1));

    if(Wheatley->y_angle>=0 && Wheatley->y_angle<10.0)
        ui->y_desired_val->setText("  "+QString::number(Wheatley->y_angle-101.0,'f',1));
    else if ((Wheatley->y_angle<0 && Wheatley->y_angle>-10.0) || Wheatley->y_angle>=10.0 )
        ui->y_desired_val->setText(" "+QString::number(Wheatley->y_angle-101.0,'f',1));
    else
        ui->y_desired_val->setText(QString::number(Wheatley->y_angle-101.0,'f',1));
        */
}

void MainWindow::handleError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError) {
        QMessageBox::critical(this, tr("Critical Error"), m_serialPort->errorString());
        closeSerialPort();
    }
}

void MainWindow::connectSignals()
{

    connect(m_serialPort, SIGNAL(errorOccurred(QSerialPort::SerialPortError)), this, SLOT(handleError(QSerialPort::SerialPortError)));
    connect(m_serialPort, SIGNAL(readyRead()), this, SLOT(readData()));

    connect(m_console, SIGNAL(getData(QByteArray)), this, SLOT(writeData(QByteArray)));

    connect(&m_senderTimer, SIGNAL(timeout()), this, SLOT(handleTimeout()));
    connect(&m_receiverTimer, SIGNAL(timeout()), this, SLOT(handleParserTimeout()));

    connect(m_gamepadController, SIGNAL(controllerNewState(SimpleXbox360Controller::InputState)), m_gamepadDisplay, SLOT(displayGamepadState(SimpleXbox360Controller::InputState)));
    connect(m_gamepadController, SIGNAL(controllerConnected(uint)), m_gamepadDisplay, SLOT(GamepadConnected()));
    connect(m_gamepadController, SIGNAL(controllerDisconnected(uint)), m_gamepadDisplay, SLOT(GamepadDisconnected()));

    connect(ui->actionConnect,  SIGNAL(triggered()), this, SLOT(openSerialPort()));
    connect(ui->actionDisconnect,  SIGNAL(triggered()), this, SLOT(closeSerialPort()));
    connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(close()));
    connect(ui->actionConfigure, SIGNAL(triggered()), m_settings, SLOT(show()));
    connect(ui->actionClear, SIGNAL(triggered()), m_console, SLOT(clear()));
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(about()));
    connect(ui->actionAboutQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

    connect(ui->xboxButton, SIGNAL(clicked()), m_gamepadDisplay, SLOT(show()));

    connect(ui->spinBox_roll, SIGNAL(valueChanged(int)), this, SLOT(on_spinBox_roll_valueChanged(int)));
    connect(ui->spinBox_tilt, SIGNAL(valueChanged(int)), this, SLOT(on_spinBox_tilt_valueChanged(int)));

    connect(ui->sliderRoll, SIGNAL(valueChanged(int)), this, SLOT(on_sliderRoll_valueChanged(int)));
    connect(ui->sliderTilt, SIGNAL(valueChanged(int)), this, SLOT(on_sliderTilt_valueChanged(int)));
}

//void MainWindow::on_actionConnect_triggered()
//{

//}

void MainWindow::handleTimeout()
{
//    char c[6] = {start_frame,joystick_frame,100,separate_frame,100,end_frame};
//    temp_PCTransfer[0] =  PCTransfer_Struct->x_desired&0x00FF;
//    temp_PCTransfer[1] = (PCTransfer_Struct->x_desired&0xFF00)>>8;
//    temp_PCTransfer[2] =  PCTransfer_Struct->y_desired&0x00FF;
//    temp_PCTransfer[3] =  (PCTransfer_Struct->y_desired&0xFF00)>>8;
//    //qDebug()<<"0 = "<<(PCTransfer_Struct->x_desired&0x00FF);
//    //qDebug()<<"2 = "<<(PCTransfer_Struct->y_desired&0x00FF);
//    c[2] = (PCTransfer_Struct->x_desired&0x00FF);
//    c[4] = (PCTransfer_Struct->y_desired&0x00FF);
//    serial->write(c,6);

    //qDebug()<<"[0]-"<<(unsigned int)(temp_PCTransfer[0]&0x00FF)<<" [2]-"<<(unsigned int)(temp_PCTransfer[2]&0x00FF)<<endl;
    //serial->write((char*)&start_frame);
   // serial->write((char*)&joystick_frame);
   // serial->write(&separate_frame);
    //serial->write((char*)&temp_PCTransfer[0]);
    //serial->write(&separate_frame);
   // serial->write((char*)&temp_PCTransfer[2]);
    //serial->write((char*)&end_frame);
}

void MainWindow::handleParserTimeout()
{
    std::cout<<"Parsing:"<<std::endl;
    if (!m_receiverQueue.empty())
    {
        char el = m_receiverQueue.front();
        //qDebug()<<"command = "<<command;
        switch(m_receiverState) {
            case NONE:
                if(static_cast<EFrame>(el) == FRAME_START) {
                    m_receiverState = CHECK;
                    m_receiverQueue.pop();
                    std::cout<<"case NONE"<<std::endl;
                }
                break;
            case CHECK:
                if(static_cast<EFrame>(el) == FRAME_TYPE_SERVO_VALUE) {
                    m_receiverState = SERVO;
                    std::cout<<"case SERVO"<<std::endl;
                } else {
                    m_receiverState = NONE;
                }
                m_receiverQueue.pop();
                //qDebug()<<"case CHECK";
                break;
            case SERVO:
                if (m_receiverQueue.size() >= 5) {
                    char servoArray[5] = {0};
                    m_receiverQueue.popN(servoArray, 5);
                    std::cout<<"Tilt: "<<std::hex<<servoArray[0]<<", "<<servoArray[1]<<std::endl;
                    std::cout<<"Roll: "<<std::hex<<servoArray[2]<<", "<<servoArray[3]<<std::endl;
                    if (static_cast<EFrame>(servoArray[4]) == FRAME_END) {
                        std::cout<<"Servo frame complete!"<<std::endl;
                        m_wheatley.tilt_servo = servoArray[0] << 8 | servoArray[1];
                        m_wheatley.roll_servo = servoArray[2] << 8 | servoArray[3];
                    }
                    m_receiverState = NONE;
                }
                break;
            default:
                m_receiverState = NONE;
                break;
        }
    }
}


void MainWindow::on_spinBox_roll_valueChanged(int roll)
{
    m_wheatley.roll_servo = roll;
    m_pcServoMsg.roll = roll;

    ui->sliderRoll->setValue(roll);
}

void MainWindow::on_spinBox_tilt_valueChanged(int tilt)
{
    m_wheatley.tilt_servo = tilt;
    m_pcServoMsg.tilt = tilt;

    ui->sliderTilt->setValue(tilt);
}

void MainWindow::on_sliderRoll_valueChanged(int roll)
{
    m_wheatley.roll_servo = roll;
    m_pcServoMsg.roll = roll;

    ui->spinBox_roll->setValue(roll);

    stickObject->setProperty("y", roll);
}

void MainWindow::on_sliderTilt_valueChanged(int tilt)
{
    m_wheatley.tilt_servo = tilt;
    m_pcServoMsg.tilt = tilt;

    ui->spinBox_tilt->setValue(tilt);

    stickObject->setProperty("x", tilt);
}

//void MainWindow::on_xboxButton_clicked()
//{

//}

void MainWindow::setControlsEnabled(bool isEnabled)
{
    ui->joystickQuickItem->setEnabled(isEnabled);
    ui->sliderRoll->setEnabled(isEnabled);
    ui->sliderTilt->setEnabled(isEnabled);
    ui->spinBox_roll->setEnabled(isEnabled);
    ui->spinBox_tilt->setEnabled(isEnabled);
    QMetaObject::invokeMethod(joyStickObject, "setEnabled", Q_ARG(QVariant, isEnabled));
}

void MainWindow::on_controller_comboBox_activated(int index)
{
    ui->spinBox_roll->setValue(SERVO_INPUT_ZERO);
    ui->spinBox_tilt->setValue(SERVO_INPUT_ZERO);

    if (index == 0) { //Screen Joystick
        ui->xboxButton->setEnabled(false);
        setControlsEnabled(true);
        disconnect(m_gamepadController, SIGNAL(controllerNewState(SimpleXbox360Controller::InputState)), this, SLOT(changeDesiredXbox(SimpleXbox360Controller::InputState)));
        connect(ui->joystickQuickItem->rootObject(), SIGNAL(joystickChanged(int,int)), this, SLOT(changeDesired(int,int)));
    }
    else if(index == 1) { //Xbox controller
        ui->xboxButton->setEnabled(true);
        setControlsEnabled(false);
        disconnect(ui->joystickQuickItem->rootObject(), SIGNAL(joystickChanged(int,int)), this, SLOT(changeDesired(int,int)));
        connect(m_gamepadController, SIGNAL(controllerNewState(SimpleXbox360Controller::InputState)), this, SLOT(changeDesiredXbox(SimpleXbox360Controller::InputState)));
    }
}

void MainWindow::changeDesiredXbox(SimpleXbox360Controller::InputState gamePadState) {
    float mul = 0.5f;

    if(gamePadState.rightTrigger > 0.5f) {
        mul = 1.f;
    } else {
        mul = 0.5f;
    }

    const float tilt = SERVO_INPUT_ZERO + SERVO_INPUT_ZERO * gamePadState.rightThumbX;
    const float roll = SERVO_INPUT_ZERO + SERVO_INPUT_ZERO * mul * gamePadState.leftThumbY;

    m_wheatley.tilt_servo = int(tilt);
    m_wheatley.roll_servo = int(roll);
    m_pcServoMsg.tilt = uint8_t(tilt);
    m_pcServoMsg.roll = uint8_t(roll);
    ui->spinBox_tilt->setValue(int(tilt));
    ui->spinBox_roll->setValue(int(roll));
}
