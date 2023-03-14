#include "gamepaddisplay.h"

GamepadDisplay::GamepadDisplay(QWidget *parent) :
    QWidget(parent),currentGamepadState(),
    gamepadConnected(false)
{
    this->setFixedSize(640,400);
}

void GamepadDisplay::GamepadConnected(){
    gamepadConnected=true;
    repaint();
}

void GamepadDisplay::GamepadDisconnected(){
    gamepadConnected=false;
    repaint();
}

void GamepadDisplay::displayGamepadState(SimpleXbox360Controller::InputState GamepadState){
    currentGamepadState=GamepadState;
    repaint();
}

void GamepadDisplay::paintEvent(QPaintEvent *event){
    Q_UNUSED(event);
    QPainter painter(this);
    if(gamepadConnected==true){
        //DRAW BACKGROUND
        painter.drawPixmap(0,0,QPixmap(":/xb360controllerBackground"));

        //DRAW BATTERY INFOS
        QString batteryText;
        batteryText.append("BATTERY TYPE : ");
        switch(currentGamepadState.batteryType){
        case BATTERY_TYPE_WIRED:
            batteryText.append("Wired");
            break;
        case BATTERY_TYPE_UNKNOWN:
            batteryText.append("Unknown");
            break;
        case BATTERY_TYPE_ALKALINE:
            batteryText.append("Alkaline");
            break;
        case BATTERY_TYPE_NIMH:
            batteryText.append("NiMH");
            break;
        case BATTERY_TYPE_DISCONNECTED:
            batteryText.append("Disconnected");
        }
        batteryText.append("\nBATTERY LEVEL : ");
        switch(currentGamepadState.batteryLevel){
        case BATTERY_LEVEL_EMPTY:
            batteryText.append("Empty");
            break;
        case BATTERY_LEVEL_LOW:
            batteryText.append("Low");
            break;
        case BATTERY_LEVEL_MEDIUM:
            batteryText.append("Medium");
            break;
        case BATTERY_LEVEL_FULL:
            batteryText.append("Full");
        }
        painter.drawText(250,360,210,40,0,batteryText);


        //DRAW BUTTONS
        if(currentGamepadState.isButtonPressed(XINPUT_GAMEPAD_A)){
            painter.setBrush(QColor("green"));
            painter.drawEllipse(487,227,40,32);
        }
        if(currentGamepadState.isButtonPressed(XINPUT_GAMEPAD_B)){
            painter.setBrush(QColor("red"));
            painter.drawEllipse(542,183,38,34);
        }
        if(currentGamepadState.isButtonPressed(XINPUT_GAMEPAD_X)){
            painter.setBrush(QColor("blue"));
            painter.drawEllipse(439,185,40,35);
        }
        if(currentGamepadState.isButtonPressed(XINPUT_GAMEPAD_Y)){
            painter.setBrush(QColor("orange"));
            painter.drawEllipse(493,142,40,34);
        }
        if(currentGamepadState.isButtonPressed(XINPUT_GAMEPAD_LEFT_SHOULDER)){
            painter.setBrush(QColor("white"));
            painter.drawEllipse(125,65,30,30);
        }
        if(currentGamepadState.isButtonPressed(XINPUT_GAMEPAD_RIGHT_SHOULDER)){
            painter.setBrush(QColor("white"));
            painter.drawEllipse(485,65,30,30);
        }
        if(currentGamepadState.isButtonPressed(XINPUT_GAMEPAD_START)){
            painter.setBrush(QColor("white"));
            painter.drawEllipse(374,195,30,22);
        }
        if(currentGamepadState.isButtonPressed(XINPUT_GAMEPAD_BACK)){
            painter.setBrush(QColor("white"));
            painter.drawEllipse(236,195,30,22);
        }
        if(currentGamepadState.isButtonPressed(XINPUT_GAMEPAD_DPAD_UP)){
            painter.setBrush(QColor("gray"));
            painter.drawEllipse(210,266,25,25);
        }
        if(currentGamepadState.isButtonPressed(XINPUT_GAMEPAD_DPAD_LEFT)){
            painter.setBrush(QColor("gray"));
            painter.drawEllipse(177,291,25,25);
        }
        if(currentGamepadState.isButtonPressed(XINPUT_GAMEPAD_DPAD_DOWN)){
            painter.setBrush(QColor("gray"));
            painter.drawEllipse(213,317,25,25);
        }
        if(currentGamepadState.isButtonPressed(XINPUT_GAMEPAD_DPAD_RIGHT)){
            painter.setBrush(QColor("gray"));
            painter.drawEllipse(245,291,25,25);
        }

        //DRAW TRIGGERS
        painter.setBrush(QColor("white"));
        painter.setPen(Qt::NoPen);
        painter.drawRect(140,58,15,-40);
        painter.drawRect(487,58,15,-40);
        QLinearGradient gradient(QPoint(0,58),QPoint(0,18));
        gradient.setColorAt(0,Qt::green);
        gradient.setColorAt(0.5,Qt::yellow);
        gradient.setColorAt(1,Qt::red);
        painter.setBrush(gradient);
        painter.drawRect(140,58,15,-40*currentGamepadState.leftTrigger);
        painter.drawRect(487,58,15,-40*currentGamepadState.rightTrigger);
        painter.setBrush(Qt::NoBrush);
        painter.setPen(Qt::SolidLine);
        painter.drawRect(140,58,15,-41);
        painter.drawRect(487,58,15,-41);

        //DRAW LEFT STICK
        QPen penLeftStick(currentGamepadState.isButtonPressed(XINPUT_GAMEPAD_LEFT_THUMB)?Qt::green:Qt::red);
        penLeftStick.setWidth(3);
        painter.setPen(penLeftStick);
        QPoint leftStickPosition(127+28*currentGamepadState.leftThumbX,226-25*currentGamepadState.leftThumbY);
        painter.drawLine(QPoint(127,226),leftStickPosition);
        painter.setBrush(currentGamepadState.isButtonPressed(XINPUT_GAMEPAD_LEFT_THUMB)?Qt::green:Qt::red);
        painter.setPen(Qt::black);
        painter.drawEllipse(leftStickPosition,5,5);

        //DRAW LEFT STICK
        QPen penRightStick(currentGamepadState.isButtonPressed(XINPUT_GAMEPAD_RIGHT_THUMB)?Qt::green:Qt::red);
        penRightStick.setWidth(3);
        painter.setPen(penRightStick);
        QPoint rightStickPosition(409+28*currentGamepadState.rightThumbX,320-25*currentGamepadState.rightThumbY);
        painter.drawLine(QPoint(409,320),rightStickPosition);
        painter.setBrush(currentGamepadState.isButtonPressed(XINPUT_GAMEPAD_RIGHT_THUMB)?Qt::green:Qt::red);
        painter.setPen(Qt::black);
        painter.drawEllipse(rightStickPosition,5,5);
    }
    else{
        painter.drawPixmap(0,0,QPixmap(":/xb360controllerShadow"));
    }
}

GamepadDisplay::~GamepadDisplay()
{
}
