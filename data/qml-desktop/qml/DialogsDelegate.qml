/*
    Copyright (c) 2013 by Ruslan Nazarov <818151@gmail.com>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*/

import QtQuick 2.0

Item {
    id: dialogsDelegate
    width: dialogsDelegate.ListView.view.width
    height: avatar.height + 13

    MouseArea {
        anchors.fill: parent
        onClicked: {
            main.dialogCurrentIndexChanged(index);
        }
    }

    HighlightListView {
        anchors.fill: parent
        visible: currentChatId === model.id
    }

    Row {
        id: dialog
        height: parent.height
        x: 10
        spacing: 7

        AvatarItem {
            id: avatar
            width: 40
            height: 40
            anchors.verticalCenter: parent.verticalCenter
            anchors.verticalCenterOffset: -1
            source: model.decoration
            online: model.online

            MouseArea {
                anchors.fill: parent
                onClicked: {
//                    if (model.uid > 0){
//                        profilePage.clear();
//                        titanim.slotShowProfile(model.uid);
//                        titanim.slotWallGet(model.uid);
//                        profilePage.uid = model.uid;
//                        mainView.pageStack.push(profilePage);
//                    }
                }
            }
        }

        Column {
            width: dialogsDelegate.width - dialog.x - avatar.width - dialog.spacing
            anchors.top: avatar.top
            anchors.topMargin: 1
            spacing: -2

            Item {
                width: parent.width
                height: name.height

                Text {
                    id: name
                    anchors.left: parent.left
                    anchors.right: messageDate.left
                    anchors.rightMargin: 5
                    color: "black"
                    font.pointSize: 13
                    font.bold: true
                    font.family: "Helvetica"
                    elide: Text.ElideRight
                    text: model.display
                }

                Text {
                    id: messageDate
                    anchors.right: parent.right
                    anchors.rightMargin: 10
                    anchors.verticalCenter: name.verticalCenter
                    color: "#1769ad"
                    font.family: "Helvetica"
                    font.pointSize: 13 - 2
                    text: model.dateStr
                }
            }

            Item {
                width: parent.width
                height: textBody.height

                Text {
                    id: textBody
                    anchors.left: parent.left
                    anchors.right: unreadCount.visible ? unreadCount.left : parent.right
                    anchors.rightMargin: 7
                    maximumLineCount: 2
                    lineHeight: 0.8
                    elide: Text.ElideRight
                    color: "#707070"
                    font.pointSize: 13 - 1
                    font.italic: model.typing
                    wrapMode: Text.Wrap
                    text: model.typing ? dialogsHandler.typingText : model.body
                }

                BadgeItem {
                    id: unreadCount
                    anchors.right: parent.right
                    anchors.rightMargin: 7
                    anchors.top: textBody.top
                    anchors.topMargin: 5
                    count: model.unreadCount
                }
            }
        }
    }

    SeparatorItem {
        id: separator
        anchors.top: dialogsDelegate.bottom
    }
}
