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
#include "SimpleXbox360Controller/simplexbox360controller.h"
#include <QFile>

QT_BEGIN_NAMESPACE

namespace Ui {
class MainWindow;
}

QT_END_NAMESPACE

class Console;
class SettingsDialog;
class Queue;
class GamepadDisplay;
class SimpleXbox360Controller;

enum Algorithm{X_NONE, X_PID, X_FUZZY, Y_NONE, Y_PID};
enum Command{NONE, CHECK, END, CONTROL, SENSOR_READY, ACC_READY, GYRO_READY, MAG_READY,
             SENSOR_COMMAND, SENDING_DN_COMMAND, SAMPLE, RECORDING_DN_COMMAND, PARAMETER_COMMAND, VOL_1, VOL_2,
             FLOAT_Y1, FLOAT_Y2, FLOAT_Y3, FLOAT_Y4,
             ACCX_1,  ACCX_2,  ACCY_1,  ACCY_2,  ACCZ_1,  ACCZ_2,
             GYROX_1, GYROX_2, GYROY_1, GYROY_2, GYROZ_1, GYROZ_2,
             MAGX_1,  MAGX_2,  MAGY_1,  MAGY_2,  MAGZ_1,  MAGZ_2,
             MEAS_1,  MEAS_2,  MEAS_3,  MEAS_4};

typedef volatile struct robotTransferStruct
{
    unsigned short int roll_servo;
    unsigned short int tilt_servo;
    unsigned short int lipol_vol;
    unsigned short int x_current;
    unsigned short int y_current;
    unsigned short int velocity;
}RobotTransferStruct;

typedef volatile struct pcTransferStruct
{
    unsigned short int x_desired;
    unsigned short int y_desired;
}PCTransferStruct;


typedef volatile struct robot_params
{
    double x_angle;
    double y_angle;
    double velocity;
    double p_gain;
    double i_gain;
    double d_gain;
    double roll_servo;
    double tilt_servo;
    double lipol_vol;
} Robot_params;

typedef struct sensorStruct
{
     short int acc[3];
     short int gyro[3];
     short int mag[3];
     int sample;
} SensorStruct;

typedef struct MeasurStruct
{
     long measur;
     int sample;
} MeasurStruct;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void openSerialPort();
    void closeSerialPort();
    void about();
    void writeData(const QByteArray &data);
    void readData();
    void changeDesired(int x,int y);
    void changeDesiredXbox(SimpleXbox360Controller::InputState GamepadState);

    void handleError(QSerialPort::SerialPortError error);

    void on_actionConnect_triggered();
    void handleTimeout();
    void handleParserTimeout();

    void on_Receive_Button_clicked();

    void on_pushButton_clicked();

    void on_spinBox_roll_valueChanged(int arg1);

    void on_spinBox_tilt_valueChanged(int arg1);

    void on_Record_Button_clicked();

    void on_xboxButton_clicked();

    void on_controller_comboBox_activated(int index);

    void on_doubleSpinBox_step_valueChanged(double arg1);

    void on_x_alg_combobox_currentIndexChanged(int index);

    void on_x_alg_combobox_activated(int index);

    void on_setpoint_button_clicked();

private:
    void initActionsConnections();
    QGroupBox *FirstGroup();
    QQuickWidget *m_quickWidget;
    QObject *rootObject;
    Robot_params *Wheatley;
    Command command;
    Algorithm ControlType[2];
    RobotTransferStruct* RobotTransfer_Struct;
    PCTransferStruct* PCTransfer_Struct;
    volatile char temp_RobotTransfer[12];
    volatile char temp_PCTransfer[4];
    SensorStruct Temp_SensorStruct;
    SensorStruct Ready_SensorStruct;
    MeasurStruct Temp_MeasurStruct;
    MeasurStruct Ready_MeasurStruct;
    FILE rec_file;
    QFile *record_file;
    volatile int iter;
    volatile int sensor_count;
    volatile long int float_y;
    double step_rec_value;
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

    int x;
    Queue* frameHandler;
    unsigned char *input;
    SimpleXbox360Controller::InputState currentGamepadState;

private:
    Ui::MainWindow *ui;
    Console *console;
    SettingsDialog *settings;
    GamepadDisplay *gamepad;
    SimpleXbox360Controller *controller;
    QSerialPort *serial;
    QTimer *senderTimer;
    QTimer *frameParserTimer;
};

#endif // MAINWINDOW_H
