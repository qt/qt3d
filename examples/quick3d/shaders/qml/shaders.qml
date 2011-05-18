import Qt 4.7

Rectangle {
    property int current : 0
    width: 640
    height: 480

    Rectangle {
        id: tabContentArea
        width: parent.width

        anchors.top: parent.top
        anchors.bottom: tabButtonArea.top
        Repeater {
            model: tabsModel
        }
    }

    VisualItemModel {
        id: tabsModel
        Collapsing { }
        //Images { }
        //Interpolate { }
        Bouncing { }
    }

    Rectangle {
        id: tabButtonArea
        height: 64
        width: parent.width

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        gradient: Gradient {
            GradientStop {position: 0.0; color: "#CCCCCC"}
            GradientStop {position: 1.0; color: "#000000"}
        }

        Component {
            id: tabButton

            Rectangle {
                height: tabButtonArea.height
                width: tabs.width / tabsModel.count

                color: "transparent"

                Image {
                    source: tabsModel.children[index].icon
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.top: parent.top
                    anchors.topMargin: 4
                }

                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: 4
                    color: "white"
                    text: tabsModel.children[index].name
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        tabClicked(index);
                    }
                }
            }
        }

        Row {
            id: tabs
            anchors.fill: parent
            Repeater {
                model: tabsModel.count
                delegate: tabButton
            }
        }
    }

    function tabClicked(index)
    {
        tabs.children[current].color = "transparent";
        tabsModel.children[current].visible = false;
        current = index;
        tabs.children[current].color = "#30ffffff";
        tabsModel.children[current].visible = true;
    }

    Component.onCompleted:
    {
        // hide all the tab views
        for(var i = 0; i < tabsModel.children.length; i++)
        {
            tabsModel.children[i].visible = false;
        }
        // select the default tab index
        tabClicked(current);
    }
}
