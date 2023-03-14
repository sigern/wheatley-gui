import QtQuick 2.0


Image {
    width: 400 ;  height:200

       Joystick{
        id:joyStick
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.bottomMargin: 10
        anchors.leftMargin: 10
        width:150;height:150
    }
}
