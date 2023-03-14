import QtQuick 2.0

Rectangle
{
    signal joystickChanged2(int x, int y);
    id: root
    color: "white"
    MyItem
    {
        id:joyStick
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        width:250;height:250
        //onClicked: joystickChanged()
    }

    Connections
    {
        target: joyStick
        onJoystickChanged: joystickChanged2(x,y)
    }
}
