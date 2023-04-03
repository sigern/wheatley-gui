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
#include "crc.h"
#include "qobjectdefs.h"
#include "ui_mainwindow.h"
#include "console.h"
#include "gamepaddisplay.h"
#include "settingsdialog.h"

#include <iostream>
#include <string>
#include <thread>
#include <QMessageBox>
#include <QQuickItem>
#include <QtSerialPort/QSerialPort>
#include <QFile>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_console = new Console;
    m_console->setEnabled(false);    
    QVBoxLayout *vbox = new QVBoxLayout;
    vbox->addWidget(m_console);
    ui->console_group->setLayout(vbox);

    QObject::connect(ui->joystickQuickItem->rootObject(), SIGNAL(joystickChanged(int, int)), this, SLOT(changeDesired(int, int)));

    m_serialPort = new QSerialPort(this);
    m_settings = new SettingsDialog;
    m_gamepadDisplay = new GamepadDisplay;
    m_gamepadController = new SimpleXbox360Controller(0);
    m_gamepadController->startAutoPolling(20);

    stickObject = ui->joystickQuickItem->rootObject()->findChild<QObject*>("stick");
    joyStickObject = ui->joystickQuickItem->rootObject()->findChild<QObject*>("joyStick");

    ui->actionConnect->setEnabled(true);
    ui->actionDisconnect->setEnabled(false);
    ui->actionQuit->setEnabled(true);
    ui->actionConfigure->setEnabled(true);
    ui->xboxButton->setEnabled(false);

    resetUiData();

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

void MainWindow::resetUiData()
{
    resetServoEnabledState();
    resetJoystickState();
    resetRobotState();
}

void MainWindow::resetServoEnabledState()
{
    m_servoEnabled = false;
    ui->startServosButton->setText("START");
}

void MainWindow::resetJoystickState()
{
    m_joystick = {JOYSTICK_ZERO, JOYSTICK_ZERO};
    ui->spinBox_roll->setValue(JOYSTICK_ZERO);
    ui->spinBox_tilt->setValue(JOYSTICK_ZERO);
    ui->sliderRoll->setValue(JOYSTICK_ZERO);
    ui->sliderTilt->setValue(JOYSTICK_ZERO);
}

void MainWindow::resetRobotState()
{
    m_wheatley = {0};
    updateUiLiveRobotParameters();
}

void MainWindow::openSerialPort()
{
    resetUiData();

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
        m_senderTimer.start(SENDER_PERIOD_IN_MS);
        m_receiverTimer.start(RECEIVER_PERIOD_IN_MS);
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
    resetUiData();

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
    const QByteArray incomingData = m_serialPort->readAll();
    const char* pIncomingCharData = incomingData.constData();
    if (!incomingData.isEmpty()) {
        std::stringstream ss;
        ss << "Queue: " << m_receiverQueue.size() << " Read " << incomingData.size() <<" bytes:";
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
}

void MainWindow::updateUiLiveRobotParameters()
{
    ui->lipol_val->setText(QString::number(m_wheatley.lipolVol, 'f', 2)+" V");
    ui->velocity_val->setText(QString::number(m_wheatley.velocity, 'f', 1));
    ui->roll_val->setText(QString::number(m_wheatley.rollServo, 'i', 1));
    ui->tilt_val->setText(QString::number(m_wheatley.tiltServo, 'i', 1));
}

void MainWindow::changeDesired(int tilt, int roll)
{
    int index = ui->controller_comboBox->currentIndex();
    if(index == 0) {
        ui->spinBox_tilt->setValue(int(tilt));
        ui->spinBox_roll->setValue(int(JOYSTICK_MAX - roll));
    }

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

    connect(&m_senderTimer, SIGNAL(timeout()), this, SLOT(handleSenderTimeout()));
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
    connect(ui->startServosButton, SIGNAL(clicked()), this, SLOT(onServoButtonClicked()));

    connect(ui->spinBox_roll, SIGNAL(valueChanged(int)), this, SLOT(on_spinBox_roll_valueChanged(int)));
    connect(ui->spinBox_tilt, SIGNAL(valueChanged(int)), this, SLOT(on_spinBox_tilt_valueChanged(int)));

    connect(ui->sliderRoll, SIGNAL(valueChanged(int)), this, SLOT(on_sliderRoll_valueChanged(int)));
    connect(ui->sliderTilt, SIGNAL(valueChanged(int)), this, SLOT(on_sliderTilt_valueChanged(int)));
}

void MainWindow::handleSenderTimeout()
{
    static int heartbeatCounter = 0;
    uint8_t crcInput[] = {m_joystick.tilt, m_joystick.roll};

    const char joystickFrame[] = {
        static_cast<char>(FRAME_START),
        static_cast<char>(FRAME_TYPE_JOYSTICK),
        static_cast<char>(m_joystick.tilt),
        static_cast<char>(m_joystick.roll),
        static_cast<char>(CRC8(crcInput, 2)),
        static_cast<char>(FRAME_END)
    };

    m_serialPort->write(joystickFrame, sizeof(joystickFrame));
    if (heartbeatCounter >= HEARTBEAT_COUNTER_MAX) {
        const char hearbeatFrame[] = {
            static_cast<char>(FRAME_START),
            static_cast<char>(FRAME_TYPE_HEARTBEAT),
            static_cast<char>(FRAME_END)
        };
        m_serialPort->write(hearbeatFrame, sizeof(hearbeatFrame));
        m_console->putData("*** Heartbeat frame sent ***\n");
        heartbeatCounter = 0;
    } else {
        heartbeatCounter++;
    }
}

void MainWindow::handleParserTimeout()
{
    if (!m_receiverQueue.empty())
    {
        char el = m_receiverQueue.front();
        std::cout<<"Receiver state: "<<m_receiverState<<" el: "<<std::hex<< int(el & 0xFF) <<std::endl;
        switch(m_receiverState) {
            case NONE:
                if (static_cast<EFrame>(el) == FRAME_START) {
                    m_receiverState = CHECK;
                    std::cout<<"case NONE"<<std::endl;
                }
                m_receiverQueue.pop();
                break;
            case CHECK:
                if (static_cast<EFrame>(el) == FRAME_TYPE_SERVO) {
                    m_receiverState = SERVO;
                    std::cout<<"case SERVO"<<std::endl;
                } else if (static_cast<EFrame>(el) == FRAME_TYPE_LIPOL) {
                    m_receiverState = LIPOL;
                    std::cout<<"case LIPOL"<<std::endl;
                } else if (static_cast<EFrame>(el) == FRAME_TYPE_VELOCITY) {
                    m_receiverState = VELOCITY;
                    std::cout<<"case VELOCITY"<<std::endl;
                } else {
                    m_receiverState = NONE;
                }
                m_receiverQueue.pop();
                break;
            case SERVO:
                if (m_receiverQueue.size() >= 5) {
                    char servoArray[5] = {0};
                    m_receiverQueue.popN(servoArray, 5);
                    if (static_cast<EFrame>(servoArray[4]) == FRAME_END) {
                        std::cout<<"Servo frame complete!"<<std::endl;
                        m_wheatley.tiltServo = uint8_t(servoArray[0]) << 8 | uint8_t(servoArray[1]);
                        m_wheatley.rollServo = uint8_t(servoArray[2]) << 8 | uint8_t(servoArray[3]);
                        updateUiLiveRobotParameters();
                    }
                    m_receiverState = NONE;
                }
                break;
            case LIPOL:
                if (m_receiverQueue.size() >= 3) {
                    char lipolArray[3] = {0};
                    m_receiverQueue.popN(lipolArray, 3);
                    if (static_cast<EFrame>(lipolArray[3]) == FRAME_END) {
                        std::cout<<"Lipol frame complete!"<<std::endl;
                        m_wheatley.lipolVol = uint8_t(lipolArray[0]) << 8 | uint8_t(lipolArray[1]);
                        updateUiLiveRobotParameters();
                    }
                    m_receiverState = NONE;
                }
                break;
            case VELOCITY:
                if (m_receiverQueue.size() >= 3) {
                    char velocityArray[3] = {0};
                    m_receiverQueue.popN(velocityArray, 3);
                    if (static_cast<EFrame>(velocityArray[3]) == FRAME_END) {
                        std::cout<<"Velocity frame complete!"<<std::endl;
                        m_wheatley.velocity = uint8_t(velocityArray[0]) << 8 | uint8_t(velocityArray[1]);
                        updateUiLiveRobotParameters();
                    }
                    m_receiverState = NONE;
                }
                break;
            default:
                m_receiverState = NONE;
                m_receiverQueue.pop();
                break;
        }
    } else {
        //std::cout<<"RX queue is empty!"<<std::endl;
    }
}


void MainWindow::on_spinBox_roll_valueChanged(int roll)
{
    m_joystick.roll = roll;
    ui->sliderRoll->setValue(roll);
}

void MainWindow::on_spinBox_tilt_valueChanged(int tilt)
{
    m_joystick.tilt = tilt;
    ui->sliderTilt->setValue(tilt);
}

void MainWindow::on_sliderRoll_valueChanged(int roll)
{
    m_joystick.roll = roll;
    ui->spinBox_roll->setValue(roll);
    stickObject->setProperty("y", JOYSTICK_MAX - roll);
}

void MainWindow::on_sliderTilt_valueChanged(int tilt)
{
    m_joystick.tilt = tilt;
    ui->spinBox_tilt->setValue(tilt);
    stickObject->setProperty("x", tilt);
}

void MainWindow::setControlsEnabled(bool isEnabled)
{
    ui->joystickQuickItem->setEnabled(isEnabled);
    ui->sliderRoll->setEnabled(isEnabled);
    ui->sliderTilt->setEnabled(isEnabled);
    ui->spinBox_roll->setEnabled(isEnabled);
    ui->spinBox_tilt->setEnabled(isEnabled);
    ui->startServosButton->setEnabled(isEnabled);
    ui->controller_comboBox->setEnabled(isEnabled);
    QMetaObject::invokeMethod(joyStickObject, "setEnabled", Q_ARG(QVariant, isEnabled));
}

void MainWindow::on_controller_comboBox_activated(int index)
{
    ui->spinBox_roll->setValue(JOYSTICK_ZERO);
    ui->spinBox_tilt->setValue(JOYSTICK_ZERO);

    if (index == 0) { // Screen Joystick
        ui->xboxButton->setEnabled(false);
        setControlsEnabled(true);
        disconnect(m_gamepadController, SIGNAL(controllerNewState(SimpleXbox360Controller::InputState)), this, SLOT(changeDesiredXbox(SimpleXbox360Controller::InputState)));
        connect(ui->joystickQuickItem->rootObject(), SIGNAL(joystickChanged(int,int)), this, SLOT(changeDesired(int,int)));
    }
    else if(index == 1) { // Xbox controller
        ui->xboxButton->setEnabled(true);
        setControlsEnabled(false);
        ui->controller_comboBox->setEnabled(true);
        disconnect(ui->joystickQuickItem->rootObject(), SIGNAL(joystickChanged(int,int)), this, SLOT(changeDesired(int,int)));
        connect(m_gamepadController, SIGNAL(controllerNewState(SimpleXbox360Controller::InputState)), this, SLOT(changeDesiredXbox(SimpleXbox360Controller::InputState)));
    }
}

void MainWindow::onServoButtonClicked()
{
    m_servoEnabled = !m_servoEnabled;
    ui->startServosButton->setText(m_servoEnabled ? "STOP" : "START");

    char servoEnabledFrame[] = {
        static_cast<char>(FRAME_START),
        static_cast<char>(FRAME_TYPE_SERVO_ENABLED),
        static_cast<char>(m_servoEnabled),
        static_cast<char>(FRAME_END)
    };
    m_serialPort->write(servoEnabledFrame, sizeof(servoEnabledFrame));
}

void MainWindow::changeDesiredXbox(SimpleXbox360Controller::InputState gamePadState) {
    float mul = 0.5f;

    if(gamePadState.rightTrigger > 0.5f) {
        mul = 1.f;
    } else {
        mul = 0.5f;
    }

    const float tilt = JOYSTICK_ZERO + JOYSTICK_ZERO * gamePadState.rightThumbX;
    const float roll = JOYSTICK_ZERO + JOYSTICK_ZERO * mul * gamePadState.leftThumbY;

    m_joystick.tilt = uint8_t(tilt);
    m_joystick.roll = uint8_t(roll);
    ui->spinBox_tilt->setValue(int(tilt));
    ui->spinBox_roll->setValue(int(roll));
}
