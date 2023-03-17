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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QQuickWidget>
#include <QQmlError>
#include <QtWidgets>
#include <QtSerialPort/QSerialPort>
#include <QGroupBox>
#include <QFile>

#include "Queue.h"
#include "SimpleXbox360Controller/simplexbox360controller.h"

QT_BEGIN_NAMESPACE

namespace Ui {
class MainWindow;
}

QT_END_NAMESPACE

class Console;
class SettingsDialog;
class GamepadDisplay;
class SimpleXbox360Controller;

enum EAlgorithm {
    X_NONE,
    X_PID,
    X_FUZZY,
    Y_NONE,
    Y_PID
};

enum EReceiverState {
    NONE,
    CHECK,
    SERVO
};
//             SENSOR_READY, ACC_READY, GYRO_READY, MAG_READY,
//             SENSOR_COMMAND, SENDING_DN_COMMAND, SAMPLE, RECORDING_DN_COMMAND, PARAMETER_COMMAND, VOL_1, VOL_2,
//             FLOAT_Y1, FLOAT_Y2, FLOAT_Y3, FLOAT_Y4,
//             ACCX_1,  ACCX_2,  ACCY_1,  ACCY_2,  ACCZ_1,  ACCZ_2,
//             GYROX_1, GYROX_2, GYROY_1, GYROY_2, GYROZ_1, GYROZ_2,
//             MAGX_1,  MAGX_2,  MAGY_1,  MAGY_2,  MAGZ_1,  MAGZ_2,
//             MEAS_1,  MEAS_2,  MEAS_3,  MEAS_4};
enum EFrame : uint8_t {
    FRAME_START = 0xF0,
    FRAME_END   = 0xF1,
    FRAME_TYPE_JOYSTICK = 0xF2,
    FRAME_TYPE_SERVO_VALUE = 0xF3
};

//static const char start_frame          = 0xFF;
//static const char end_frame            = 0xFE;
//static const char separate_frame       = 0xFA;
//static const char joystick_frame       = 0xFD;
//static const char pid_frame            = 0xFC;
//static const char control_frame        = 0xFB;
//static const char sensor_frame         = 0xF9;
//static const char receive_frame        = 0xF8;
//static const char sending_done_frame   = 0xF7;
//static const char record_frame         = 0xF6;
//static const char recording_done_frame = 0xF5;
//static const char parameter_frame      = 0xF0;
//static const char alg_frame            = 0xEF;

typedef struct robotMsg
{
    unsigned int roll_servo;
    unsigned int tilt_servo;
    unsigned int lipol_vol;
    unsigned int x_current;
    unsigned int y_current;
    unsigned int velocity;
} RobotMsg_t;

typedef struct pcServoMsg
{
    uint8_t tilt = 100;
    uint8_t roll = 100;
} PcServoMsg_t;

typedef struct robotState
{
    int roll_servo = 100;
    int tilt_servo = 100;
    float x_angle;
    float y_angle;
    float velocity;
    float lipol_vol;
    float p_gain;
    float i_gain;
    float d_gain;
} RobotState_t;

typedef struct sensors
{
     short int acc[3];
     short int gyro[3];
     short int mag[3];
     int sample;
} Sensors_t;

//typedef struct MeasurStruct
//{
//     long measur;
//     int sample;
//} sMeasurment;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    static const char start_frame          = 0xFF;
    static const char end_frame            = 0xFE;
    static const char separate_frame       = 0xFA;
    static const char joystick_frame       = 0xFD;
    static const char pid_frame            = 0xFC;
    static const char control_frame        = 0xFB;
    static const char sensor_frame         = 0xF9;
    static const char receive_frame        = 0xF8;
    static const char sending_done_frame   = 0xF7;
    static const char record_frame         = 0xF6;
    static const char recording_done_frame = 0xF5;
    static const char parameter_frame      = 0xF0;
    static const char alg_frame            = 0xEF;

    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void openSerialPort();
    void closeSerialPort();
    void about();
    void writeData(const QByteArray &data);
    void readData();
    void changeDesired(int tilt, int roll);
    void changeDesiredXbox(SimpleXbox360Controller::InputState GamepadState);

    void handleError(QSerialPort::SerialPortError error);

    //void on_actionConnect_triggered();
    void handleTimeout();
    void handleParserTimeout();

    void on_spinBox_roll_valueChanged(int arg1);
    void on_spinBox_tilt_valueChanged(int arg1);
    //void on_xboxButton_clicked();
    void on_controller_comboBox_activated(int index);

private:
    void connectSignals();
    void updateUiControlValues();

    QGroupBox *FirstGroup();
    QQuickWidget *m_quickWidget;
    QObject *rootObject;

    Ui::MainWindow *ui;
    Console *m_console;
    SettingsDialog *m_settings;
    GamepadDisplay *m_gamepadDisplay;
    SimpleXbox360Controller *m_gamepadController;
    QSerialPort *m_serialPort;

    QTimer m_senderTimer;
    QTimer m_receiverTimer;

    RobotState_t m_wheatley;
    EReceiverState m_receiverState = EReceiverState::NONE;
    //Algorithm ControlType[2];

    RobotMsg_t m_robotMsg;
    PcServoMsg_t m_pcServoMsg;

    FixedQueue<char, 1000> m_receiverQueue;
    unsigned char *input;
    SimpleXbox360Controller::InputState m_gamepadInputState;

    //    volatile char temp_RobotTransfer[12];
    //    volatile char temp_PCTransfer[4];
    //    SensorStruct Temp_SensorStruct;
    //    SensorStruct Ready_SensorStruct;
    //    MeasurStruct Temp_MeasurStruct;
    //    MeasurStruct Ready_MeasurStruct;
    //    FILE rec_file;
    //    QFile *record_file;
    //    volatile int iter;
    //    volatile int sensor_count;
    //    volatile long int float_y;
    //    double step_rec_value;
    //    int x;
};

#endif // MAINWINDOW_H
