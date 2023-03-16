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
#include "ui_mainwindow.h"
#include "console.h"
#include "gamepaddisplay.h"
#include "settingsdialog.h"

#include <iostream>
#include <string>
#include <QMessageBox>
#include <QQuickItem>
#include <QtSerialPort/QSerialPort>
#include "Queue.h"
#include <QFile>


//! [0]
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_quickWidget(new QQuickWidget),
    Wheatley(new Robot_params),
    ui(new Ui::MainWindow)
{
//! [0]
    this->setFixedSize(810,610);
    ui->setupUi(this);
    console = new Console;
    console->setEnabled(false);
    //setCentralWidget(console);
    QVBoxLayout *vbox = new QVBoxLayout;
    vbox->addWidget(console);
    ui->console_group->setLayout(vbox);
    ui->console_group->setFixedHeight(300);
    ui->console_group->setFixedWidth(480);
    //ui->control_group->setFixedHeight(210);

    QUrl source("qrc:/Joystick.qml");
    QVBoxLayout *vbox_joy = new QVBoxLayout;
    vbox_joy->addWidget(m_quickWidget);
    ui->joystick_group->setLayout(vbox_joy);
    ui->joystick_group->setFixedHeight(300);
    ui->joystick_group->setFixedWidth(300);
    m_quickWidget ->resize(100,100);
    m_quickWidget->setResizeMode(QQuickWidget::SizeRootObjectToView );
    m_quickWidget->setSource(source);
    ui->control_group->setFixedHeight(250);

    rootObject = dynamic_cast<QObject*>(m_quickWidget->rootObject());
    QObject::connect(rootObject, SIGNAL(joystickChanged(int,int)), this, SLOT(changeDesired(int,int)));

    ui->lipol_val->setText(QString::number(0,'f',2)+" V");
    ui->roll_val->setText(QString::number(0,'f',1));
    ui->tilt_val->setText(QString::number(0,'f',1));
    ui->velocity_val->setText(QString::number(0,'f',1));
    ui->x_current_val->setText("  "+QString::number(0,'f',1));
    ui->y_current_val->setText("  "+QString::number(0,'f',1));
    ui->spinBox_roll->setValue(0);
    ui->spinBox_tilt->setValue(0);
//    ui->p_value->setText(QString::number(0,'f',1));
//    ui->i_value->setText(QString::number(0,'f',1));
//    ui->d_value->setText(QString::number(0,'f',1));

    frameHandler = new Queue;

   // setCentralWidget(centralWidget);
//! [1]
    serial = new QSerialPort(this);
//! [1]
    settings = new SettingsDialog;
    gamepad = new GamepadDisplay;
    controller = new SimpleXbox360Controller(0);
    controller->startAutoPolling(20);
    senderTimer = new QTimer;
    frameParserTimer = new QTimer;

    ui->actionConnect->setEnabled(true);
    ui->actionDisconnect->setEnabled(false);
    ui->Disconnect_button->setEnabled(false);
    ui->actionQuit->setEnabled(true);
    ui->actionConfigure->setEnabled(true);
    ui->xboxButton->setEnabled(false);

    initActionsConnections();

    connect(serial, SIGNAL(errorOccurred(QSerialPort::SerialPortError)), this,
            SLOT(handleError(QSerialPort::SerialPortError)));

//! [2]
    connect(serial, SIGNAL(readyRead()), this, SLOT(readData()));
//! [2]
    connect(console, SIGNAL(getData(QByteArray)), this, SLOT(writeData(QByteArray)));
    connect(senderTimer, SIGNAL(timeout()), this, SLOT(handleTimeout()));
    connect(frameParserTimer, SIGNAL(timeout()), this, SLOT(handleParserTimeout()));

    command = NONE;
    RobotTransfer_Struct = new RobotTransferStruct;
    RobotTransfer_Struct->roll_servo = 0;
    RobotTransfer_Struct->tilt_servo = 0;
    RobotTransfer_Struct->lipol_vol = 0;
    RobotTransfer_Struct->x_current = 100;
    RobotTransfer_Struct->y_current = 100;
    RobotTransfer_Struct->velocity = 0;

    PCTransfer_Struct = new PCTransferStruct;
    PCTransfer_Struct->x_desired = 100;
    PCTransfer_Struct->y_desired = 100;

    for (int i = 0;i<12;i++)
        temp_RobotTransfer[i] = 0;
    for (int j = 0;j<4;j++)
        temp_PCTransfer[j] = 0;
    iter = 0;
    sensor_count = 0;
    x=0;
    input = new unsigned char;
    record_file = new QFile();
    step_rec_value = 0.0;
    float_y = 0;
    ControlType[0] = X_NONE;
    ControlType[1] = Y_NONE;
    //! [3]
}
//! [3]

MainWindow::~MainWindow()
{
    delete settings;
    delete gamepad;
    delete controller;
    delete ui;
    delete serial;
    delete senderTimer;
    delete frameParserTimer;
    delete console;
    delete record_file;
}

//! [4]
void MainWindow::openSerialPort()
{
    SettingsDialog::Settings p = settings->settings();
    serial->setPortName(p.name);
    serial->setBaudRate(p.baudRate);
    serial->setDataBits(p.dataBits);
    serial->setParity(p.parity);
    serial->setStopBits(p.stopBits);
    serial->setFlowControl(p.flowControl);
    if (serial->open(QIODevice::ReadWrite)) {
        console->setEnabled(true);
        console->setLocalEchoEnabled(p.localEchoEnabled);
        ui->actionConnect->setEnabled(false);
        ui->Connect_button->setEnabled(false);
        ui->Configure_button->setEnabled(false);
        ui->actionConfigure->setEnabled(false);
        ui->actionDisconnect->setEnabled(true);
        ui->Disconnect_button->setEnabled(true);
        ui->statusBar->showMessage(tr("Connected to %1 : %2, %3, %4, %5, %6")
                                   .arg(p.name).arg(p.stringBaudRate).arg(p.stringDataBits)
                                   .arg(p.stringParity).arg(p.stringStopBits).arg(p.stringFlowControl));
        console->putData("*** Connection successfully established ***\n");

        //senderTimer->start(1);
        //frameParserTimer->start(3);
    }
    else
    {
        QMessageBox::critical(this, tr("Error"), serial->errorString());

        ui->statusBar->showMessage(tr("Open error"));
        console->putData("*** Connection encountered an error ***\n");

    }
    iter = 0;
    RobotTransfer_Struct->roll_servo = 0;
    RobotTransfer_Struct->tilt_servo = 0;
    RobotTransfer_Struct->lipol_vol = 0;
    RobotTransfer_Struct->x_current = 100;
    RobotTransfer_Struct->y_current = 100;
    RobotTransfer_Struct->velocity = 0;
    PCTransfer_Struct->x_desired = 100;
    PCTransfer_Struct->y_desired = 100;
    for (int i = 0;i<12;i++)
        temp_RobotTransfer[i] = 0;
    for (int j = 0;j<4;j++)
        temp_PCTransfer[j] = 0;

}
//! [4]

//! [5]
void MainWindow::closeSerialPort()
{
    serial->close();
    console->setEnabled(false);
    ui->actionConnect->setEnabled(true);
    ui->Connect_button->setEnabled(true);
    ui->Disconnect_button->setEnabled(false);
    ui->actionDisconnect->setEnabled(false);
    ui->actionConfigure->setEnabled(true);
    ui->Configure_button->setEnabled(true);
    ui->statusBar->showMessage(tr("Disconnected"));
    console->putData("*** Connection aborted by user ***\n");
    senderTimer->stop();
    frameParserTimer->stop();
    sensor_count=0;
}
//! [5]

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

//! [6]
void MainWindow::writeData(const QByteArray &data)
{
    serial->write(data);
}
//! [6]

//! [7]
void MainWindow::readData()
{
//    static const unsigned char start_frame = 0xFF;
//    static const unsigned char end_frame = 0xFE;
//    static const unsigned char separate_frame = 0xFA;
//    static const unsigned char joystick_frame = 0xFD;
//    static const unsigned char pid_frame = 0xFC;
//    static const unsigned char control_frame =0xFB;

    //qDebug()<<"Read"<<endl;
    QByteArray data = serial->readAll();
    //qDebug()<<"Length:"<<data.toStdString().length()<<endl;
        for(unsigned int z=0; z<data.toStdString().length(); z++)
        {
          // if((frameHandler->Push((char)data.data()[z]))==-1);
              //qDebug()<<"RECEIVE COMMAND QUEUE FULL\n";
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
//! [7]
void MainWindow::changeDesired(int x,int y)
{
//    ui->x_desired_val->setText(QString::number(x,'f',1));
//    ui->y_desired_val->setText(QString::number((-y+200),'f',1));
    int index = ui->controller_comboBox->currentIndex();
    if(index == 0)
    {
        ui->spinBox_roll->setValue(-y+200);
        ui->spinBox_tilt->setValue(x);

        Wheatley->x_angle = (double(x));
        Wheatley->y_angle = (-double(y))+200;

        PCTransfer_Struct->x_desired = Wheatley->x_angle;
        PCTransfer_Struct->y_desired = Wheatley->y_angle;
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

//! [8]
void MainWindow::handleError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError) {
        QMessageBox::critical(this, tr("Critical Error"), serial->errorString());
        closeSerialPort();
    }
}
//! [8]

void MainWindow::initActionsConnections()
{
    connect(ui->Connect_button, SIGNAL(clicked()), this, SLOT(openSerialPort()));
    connect(ui->actionConnect, SIGNAL(triggered()), this, SLOT(openSerialPort()));

    connect(ui->Disconnect_button, SIGNAL(clicked()), this, SLOT(closeSerialPort()));
    connect(ui->actionDisconnect, SIGNAL(triggered()), this, SLOT(closeSerialPort()));

    connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(close()));

    connect(ui->Configure_button, SIGNAL(clicked()), settings, SLOT(show()));
    connect(ui->actionConfigure, SIGNAL(triggered()), settings, SLOT(show()));

    connect(ui->Clear_button, SIGNAL(clicked()), console, SLOT(clear()));
    connect(ui->actionClear, SIGNAL(triggered()), console, SLOT(clear()));
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(about()));
    connect(ui->actionAboutQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

    connect(ui->xboxButton, SIGNAL(clicked()), gamepad, SLOT(show()));

    connect(controller,SIGNAL(controllerNewState(SimpleXbox360Controller::InputState)),gamepad,SLOT(displayGamepadState(SimpleXbox360Controller::InputState)));
    connect(controller,SIGNAL(controllerConnected(uint)),gamepad,SLOT(GamepadConnected()));
    connect(controller,SIGNAL(controllerDisconnected(uint)),gamepad,SLOT(GamepadDisconnected()));

}

void MainWindow::on_actionConnect_triggered()
{

}

void MainWindow::handleTimeout()
{
    char c[6] = {start_frame,joystick_frame,100,separate_frame,100,end_frame};
    temp_PCTransfer[0] =  PCTransfer_Struct->x_desired&0x00FF;
    temp_PCTransfer[1] = (PCTransfer_Struct->x_desired&0xFF00)>>8;
    temp_PCTransfer[2] =  PCTransfer_Struct->y_desired&0x00FF;
    temp_PCTransfer[3] =  (PCTransfer_Struct->y_desired&0xFF00)>>8;
    //qDebug()<<"0 = "<<(PCTransfer_Struct->x_desired&0x00FF);
    //qDebug()<<"2 = "<<(PCTransfer_Struct->y_desired&0x00FF);
    c[2] = (PCTransfer_Struct->x_desired&0x00FF);
    c[4] = (PCTransfer_Struct->y_desired&0x00FF);
    serial->write(c,6);

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

    char check_empty = 0;
    //qDebug()<<"1";
    check_empty = frameHandler->Pop(input);
    //qDebug()<<"2";
    if (check_empty != -1)
    {
        //qDebug()<<"command = "<<command;
        switch(command)
        {
        case NONE:
            if(*input == FRAME_START)
            {
                command = CHECK;
                //qDebug()<<"case NONE";
            }
            break;
        case CHECK:
            if(*input == FRAME_TYPE_SERVO_VALUE)
            {
                command = SENSOR_COMMAND;
                //qDebug()<<"sensor";
            }
            else if (*input == sending_done_frame)
            {
                command = SENDING_DN_COMMAND;
               //qDebug()<<"send done";
            }
            else if (*input == parameter_frame)
            {
                command = PARAMETER_COMMAND;
                //qDebug()<<"parameter done";
            }
            else if (*input == recording_done_frame)
            {
                command = RECORDING_DN_COMMAND;
                //qDebug()<<"rec done";
                record_file->setFileName("wheatley_rec.txt");
                record_file->open(QIODevice::ReadWrite | QIODevice::Text);
                //record_file->write("gyro_x\tacc_y\tacc_z\t\n");
            }
            else
                command = NONE;

            //qDebug()<<"case CHECK";
            break;
        }
    }
    //else
        //qDebug()<<"empty";

    //qDebug()<<"end";
}


void MainWindow::on_spinBox_roll_valueChanged(int roll)
{
    Wheatley->y_angle = (double(roll));
    PCTransfer_Struct->y_desired = Wheatley->y_angle;
}

void MainWindow::on_spinBox_tilt_valueChanged(int tilt)
{
    Wheatley->x_angle = (double(tilt));
    PCTransfer_Struct->x_desired = Wheatley->x_angle;
}

void MainWindow::on_xboxButton_clicked()
{

}


void MainWindow::on_controller_comboBox_activated(int index)
{
    if (index == 0) //Screen Joystick
    {
        ui->xboxButton->setEnabled(false);
        QObject::connect(rootObject, SIGNAL(joystickChanged(int,int)), this, SLOT(changeDesired(int,int)));
        disconnect(controller,SIGNAL(controllerNewState(SimpleXbox360Controller::InputState)),this,SLOT(changeDesiredXbox(SimpleXbox360Controller::InputState)));

    }
    else if(index == 1) //Xbox controller
    {
        ui->xboxButton->setEnabled(true);
        QObject::disconnect(rootObject, SIGNAL(joystickChanged(int,int)), this, SLOT(changeDesired(int,int)));
        connect(controller,SIGNAL(controllerNewState(SimpleXbox360Controller::InputState)),this,SLOT(changeDesiredXbox(SimpleXbox360Controller::InputState)));
    }
    else if(index == 2) //Step while recording
    {
        QObject::disconnect(rootObject, SIGNAL(joystickChanged(int,int)), this, SLOT(changeDesired(int,int)));
    }
}

void MainWindow::changeDesiredXbox(SimpleXbox360Controller::InputState GamepadState){
    currentGamepadState=GamepadState;
    float mul=0.5;
    if(currentGamepadState.rightTrigger>0.5)
        mul=1;
    else
        mul=0.5;
    Wheatley->x_angle = 100+currentGamepadState.rightThumbX*100;
    Wheatley->y_angle = (100+mul*currentGamepadState.leftThumbY*100);

    PCTransfer_Struct->x_desired = Wheatley->x_angle;
    PCTransfer_Struct->y_desired = Wheatley->y_angle;
    ui->spinBox_roll->setValue(PCTransfer_Struct->y_desired);
    ui->spinBox_tilt->setValue(PCTransfer_Struct->x_desired);
}
