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

constexpr uint8_t JOYSTICK_MIN = 0;
constexpr uint8_t JOYSTICK_ZERO = 120;
constexpr uint8_t JOYSTICK_MAX = 240;

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
    ROBOT_STATE,
    LIPOL
};

enum EFrame : uint8_t {
    FRAME_START = 0xF0,
    FRAME_END   = 0xF1,
    FRAME_TYPE_JOYSTICK = 0xF2,
    FRAME_TYPE_ROBOT_STATE = 0xF3,
    FRAME_TYPE_LIPOL = 0xF4,
    FRAME_TYPE_VELOCITY = 0xF5,
    FRAME_TYPE_TILT_CONTROL_ENABLED = 0xF6,
    FRAME_TYPE_HEARTBEAT = 0xF7
};

typedef struct joystickState
{
    uint8_t tilt = JOYSTICK_ZERO;
    uint8_t roll = JOYSTICK_ZERO;
} JoystickState_t;

typedef struct robotState
{
    uint16_t tiltServo = 0u;
    uint16_t rollServo = 0u;
    int16_t tiltAngle = 0.f;
    int16_t rollAngle = 0.f;
    int16_t velocity = 0.f;
    uint16_t lipolVol = 0.f;
} RobotState_t;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    static const unsigned char JOYSTICK_MIN = 0;
    static const unsigned char JOYSTICK_ZERO = 120;
    static const unsigned char JOYSTICK_MAX = 240;

    static const int SENDER_PERIOD_IN_MS = 20;
    static const int RECEIVER_PERIOD_IN_MS = 5;
    static const int HEARTBEAT_COUNTER_MAX = 1000 / SENDER_PERIOD_IN_MS;

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

    void handleSenderTimeout();
    void handleParserTimeout();

    void on_spinBox_roll_valueChanged(int arg1);
    void on_spinBox_tilt_valueChanged(int arg1);
    void on_sliderRoll_valueChanged(int roll);
    void on_sliderTilt_valueChanged(int tilt);
    void on_controller_comboBox_activated(int index);
    void onServoButtonClicked();

    void on_comboBoxTiltControl_currentIndexChanged(int index);

private:
    void setControlsEnabled(bool isEnabled);
    void connectSignals();
    void updateUiLiveRobotParameters();
    void resetUiData();
    void resetTiltControlEnabledState();
    void resetJoystickState();
    void resetRobotState();

    QObject *stickObject;
    QObject *joyStickObject;

    Ui::MainWindow *ui;
    Console *m_console;
    SettingsDialog *m_settings;
    GamepadDisplay *m_gamepadDisplay;
    SimpleXbox360Controller *m_gamepadController;
    QSerialPort *m_serialPort;

    QTimer m_senderTimer;
    QTimer m_receiverTimer;

    RobotState_t m_wheatley;
    JoystickState_t m_joystick;
    bool m_tiltControlEnabled = false;
    EReceiverState m_receiverState = EReceiverState::NONE;

    FixedQueue<char, 100> m_receiverQueue;
    unsigned char *input;
    SimpleXbox360Controller::InputState m_gamepadInputState;
};

#endif // MAINWINDOW_H
