import Qt 4.7

Rectangle {
    id: rectangle1
    width: 900
    height: 800
    BorderImage {
        id: name
        anchors.fill: parent
        source: "images/bg.png"
        border.left: 15; border.top:40
        border.right: 15; border.bottom: 15

    }
    BorderImage {
        x: -30

        id: name2
        source: "images/section.png"
        width: Math.min (name.width+30, (name.height-20)/1.14)
        height:  Math.min (name.height-20, (name.width+30)*1.14   )
//        clip: true
        anchors.verticalCenter: parent.verticalCenter
        border.left: 125; border.top: 130
        border.right: 50; border.bottom: 72
        verticalTileMode: BorderImage.Repeat

        Image {
            id: next
            source: "images/next.png"
            x: name2.width-80
            y: name2.height-90
            MouseArea {
                anchors.fill: parent
                onClicked: { rect1.width=name2.width-106 }

            }
        }
        Image {
            id: prev
            source: "images/prev.png"
            x: 100
            y: name2.height-90
            MouseArea {
                anchors.fill: parent
                onClicked: {  rectleft1.width=name2.width-109 }
            }}


        Item {id: leftflip

        Item{
            id: rect1
            x: name2.width-44-rect1.width
            y: 23
            width: 0
            height:name2.height-51
            clip: true
            opacity: rect1.width==name2.width-106? 0: 1
            z:rect1.opacity==0? 3:2 //trick to force the animation to get back to 0
            Behavior on width {
                NumberAnimation {duration: rect1.width==name2.width-106? 0 :1200; easing.type: Easing.InCubic}
            }
            Behavior on opacity {
            NumberAnimation {duration: 300 }
        }
            onZChanged: {rect1.width=0}
            Image {
                id: backshadow
                anchors.left: parent.left
                anchors.leftMargin: 0
                source: "images/backshadow.png"
                height: parent.height
                smooth: true
                width: Math.min ( rect1.width, 80 )

            }
        }
        Image {id:dropshadow

            y: 23
            x: rect2.x-(Math.min ( rect1.width, 19 ))
            height:rect2.height
            smooth: true
            opacity: rect1.opacity
            source: "images/dropshadow.png"
            width:Math.min ( rect1.width, 19 )
        }
        Item{
            id: rect2
            x: rect1.x-rect1.width+1
            y: 23
            width:rect1.width
            height:name2.height-51
            opacity: rect1.opacity
            clip: true
            BorderImage {
                id: rev
                source: "images/rev.png"
                width: name2.width-44; height:name2.height-51
                border.left: 0; border.top: 105
                border.right: 0; border.bottom: 46
                verticalTileMode: BorderImage.Repeat
            }
            Image {
                id: topshadow
                anchors.right: parent.right

                source: "images/topshadow.png"
                height: parent.height
                smooth: true
                width: Math.min ( rect2.width, 152 )

            }
        }
    }
        Item {id: rightflip
            opacity: rectleft2.x==64? 0:1

            Behavior on opacity {
            NumberAnimation {duration: 200 }
        }
            Item {

            id: rectleft1
            x:(-name2.width+173)
            y:23
            width: 0

            height:name2.height-51

            Behavior on width {
                NumberAnimation {duration:rectleft2.x==64? 0:1300; easing.type: Easing.OutCubic }
            }
            }
            Item{
            z:rightflip.opacity==0? 2:3
            onZChanged: {rectleft1.width=0}

            id: rectleft2
            x:rectleft1.x+rectleft1.width
            y:rectleft1.y+1
            width:rectleft1.width
            height:rectleft1.height


            BorderImage {
                id: sob
                 smooth: true
                source: "images/paper.png"
                anchors.fill: parent
                border.left: 62; border.top: 105
                border.right: 0; border.bottom: 46
                verticalTileMode: BorderImage.Repeat
            }
            Image {
                anchors.bottom: parent.bottom
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.bottomMargin: 0
                anchors.topMargin: 0
                anchors.leftMargin: 0
 smooth: true
                source: "images/topshadowright.png"

            }

            }
            Image {
            id:rectleft3
            x:rectleft2.x+rectleft1.width-1
            y:rectleft1.y
            source: "images/backshadow.png"
            width:Math.min(60,(name2.width-105-rectleft1.width) )
            height:rectleft1.height
            smooth: true
        }
        }


    }
}
