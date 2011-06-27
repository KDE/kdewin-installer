import QtQuick 1.0

Rectangle {
    id: rectangle2
    width: 600
    height: 480
    color: "#443c3c"

    Rectangle {
        id: rectangle1
        x: -159
        y: 159
        width: 480
        height: 162
        radius: 0
        opacity: 1
        gradient: Gradient {
            GradientStop {
                position: 0
                color: "#938989"
            }

            GradientStop {
                position: 0.94
                color: "#000000"
            }
        }
        rotation: 90
    }
    ListModel{
        id: softwareModel
        ListElement{
            name: "Amarok"
            pack: "amarok"
            repo: "some random repo"
        }
        ListElement{
            name: "Kopete"
            pack: "kopete"
            repo: "another repo"
        }
    }

    ListView {
        id: list_view1
        x: 162
        y: 0
        width: 438
        height: 480
        anchors.verticalCenterOffset: 0
        anchors.right: parent.right
        anchors.rightMargin: 0
        anchors.verticalCenter: parent.verticalCenter
        opacity: 1
        model: softwareModel
        delegate: Rectangle{
            width: 438
            height: 84
            color: "#5e5252"
            radius: 32
            anchors.rightMargin: parent.right
            Text {
                text: name
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter
            }
        }
    }
}
