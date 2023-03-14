#ifndef GAMEPADDISPLAY_H
#define GAMEPADDISPLAY_H

#include <QWidget>
#include <QPainter>
#include <QImage>
#include <QPixmap>
#include <QString>
#include <QBrush>
#include <QColor>
#include <QLinearGradient>
#include <QPoint>
#include <QPen>
#include <QMouseEvent>
#include <QDebug>
#include <QDialog>
#include "SimpleXbox360Controller/simplexbox360controller.h"


class GamepadDisplay : public QWidget
{
    Q_OBJECT
    
public:
    explicit GamepadDisplay(QWidget *parent = 0);
    ~GamepadDisplay();

public slots:
    void displayGamepadState(SimpleXbox360Controller::InputState GamepadState);
    void GamepadConnected(void);
    void GamepadDisconnected(void);

protected:
    void paintEvent(QPaintEvent *);
private:
    SimpleXbox360Controller::InputState currentGamepadState;
    bool gamepadConnected;
};

#endif // GAMEPADDISPLAY_H
