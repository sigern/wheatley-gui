import QtQuick 2.0

Rectangle
{
    signal joystickChanged(int x, int y);
    id: root
    color: "white"
    MyItem
    {
        id:joyStick
        objectName:"joyStick"
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        width:300;height:300
        //onClicked: joystickChanged()
    }

    Connections
    {
        target: joyStick
        function onJoystickChanged(x, y) { joystickChanged(x, y) }
    }
}
