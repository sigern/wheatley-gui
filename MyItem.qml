import QtQuick 2.0

Item {
    id:joyStick;

    signal joystickChanged(int x, int y);

    Rectangle {
       id:entireArea
       color:"dimgray"
       width:parent.width;height:parent.height
    }

    Rectangle {
        id:totalArea
        color:"black"
        radius: parent.width/2
        width:parent.width
        height:parent.height
    }

    Rectangle{
        id:stick
        objectName:"stick"
        width:totalArea.width/5; height: width
        radius: width/2
        x: totalArea.width/2 - radius
        y: totalArea.height/2 - radius
        color:"#FF398585"
    }

    MouseArea{
        id:mouseArea
        anchors.fill: parent

        onPressed: {
            stick.color = "#FF67F0F0"
        }

        onReleased: {
            stick.color = "#FF398585"
            //snap to center
            stick.x = totalArea.width /2 - stick.radius;
            stick.y = totalArea.height/2 - stick.radius;
            joyStick.joystickChanged(stick.x,stick.y);
        }

        onPositionChanged:
        {
             //(x-center_x)^2 + (y - center_y)^2 < radius^2
             //if stick need to remain inside larger circle
             //var rad = (totalArea.radius - stick.radius);
             //if stick can go outside larger circle
             var rad = totalArea.radius-stick.radius;
             rad =  rad * rad;

             // calculate distance in x direction
             var xDist = mouseX - (totalArea.x + totalArea.radius);
             xDist = xDist * xDist;

             // calculate distance in y direction
             var yDist = mouseY - (totalArea.y + totalArea.radius);
             yDist = yDist * yDist;

             //total distance for inner circle
             var dist = xDist + yDist;

            var over;
            /*
            //Circle mouse area
            if(dist>rad)
            {
                over = 1;
            }
            else
            {
                over = 0;
            }
            */
            //Rectangle mouse area
            if(yDist>rad || xDist>rad)
            {
                over = 1;
            }
            else
            {
                over = 0;
            }

             //center of larger circle
             var oldX = stick.x; var oldY = stick.y;
             var R = totalArea.radius-stick.radius;
             var Y = mouseY-totalArea.height/2;
             var X = mouseX-totalArea.width/2;
             if (over==1)
             {
                 /*
                 // cirlce mouse area
                 stick.x = totalArea.width/2  - stick.radius + R*(X/Math.sqrt(X*X+Y*Y));
                 stick.y = totalArea.height/2 - stick.radius + R*(Y/Math.sqrt(X*X+Y*Y));
                 */

                 //Rectangle mouse area
                 stick.x = mouseX-stick.radius;
                 stick.y = mouseY-stick.radius;
                 if(X>R)
                     stick.x = totalArea.width/2  - stick.radius + R;
                 else if(X<-R)
                     stick.x = 0;
                 if(Y>R)
                     stick.y = totalArea.height/2  - stick.radius + R;
                 else if(Y<-R)
                     stick.y = 0;
             }
             else
             {
                 stick.x = mouseX-stick.radius;
                 stick.y = mouseY-stick.radius;
             }

            joyStick.joystickChanged(stick.x, stick.y);
        }

        onDoubleClicked:
        {
            //snap to center
            stick.x = totalArea.width /2 - stick.radius;
            stick.y = totalArea.height/2 - stick.radius;
            joyStick.joystickChanged(stick.x,stick.y);
        }
    }

    function setEnabled(isEnabled) {
        if(isEnabled)
        {
            totalArea.color = "black"
            stick.color = "#FF398585"
        }
        else
        {
            totalArea.color = "grey"
            stick.color = "dimgray"
        }
    }

    Text
    {
        anchors.verticalCenter: totalArea.verticalCenter;
        anchors.left: totalArea.left;
        anchors.leftMargin: 4;
        text: "Left"
        color:"white"
    }
    Text
    {
        anchors.horizontalCenter: totalArea.horizontalCenter;
        anchors.top: totalArea.top;
        anchors.topMargin: 4;
        text: "Forward"
        color:"white"
    }
    Text
    {
        anchors.verticalCenter: totalArea.verticalCenter;
        anchors.right: totalArea.right;
        anchors.rightMargin: 4;
        text: "Right"
        color:"white"
    }
    Text
    {
        anchors.horizontalCenter: totalArea.horizontalCenter;
        anchors.bottom: totalArea.bottom;
        anchors.bottomMargin: 4;
        text: "Backward"
        color:"white"
    }
    Text
    {
        anchors.horizontalCenter: totalArea.horizontalCenter;
        anchors.verticalCenter: totalArea.verticalCenter;
        text: "+"
        color: "white"
    }
}
