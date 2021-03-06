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

import QtQuick 2.1

FocusScope {
    id: root

    property int currentIndex: 0
    property int count: 0
    property ListModel __tabs: ListModel {}
    property Component widget
    property bool visibleWidget: false

    function removeTab(index) {
        var tab = __tabs.get(index).tab
        __tabs.remove(index, 1)
        tab.destroy()

        if (currentIndex > 0)
            currentIndex--

        __setOpacities()
    }

    function currentItemTitle() {
        return root.__tabs.get(currentIndex).tab.title
    }

    function setCurrentItem(title) {
        for (var i = 0; i < __tabs.count; ++i) {
            var child = __tabs.get(i).tab

            if (child.title === title) {
                __setCurrentItem(i, child)
            }
        }
    }

    function __setOpacities() {
        for (var i = 0; i < __tabs.count; ++i) {
            var child = __tabs.get(i).tab
            child.visible = (i == currentIndex ? true : false)
        }

        count = __tabs.count
    }

    function __setCurrentItem(index, tab) {
        if (root.currentIndex !== index) {
            __tabs.get(root.currentIndex).tab.visible = false
        }

        tab.clicked()
        root.currentIndex = index
    }

    Rectangle {
        id: t
        implicitWidth: 50
        implicitHeight: parent.height
        anchors.left: parent.left
        color: "#4c4c4d"

        ListView {
            id: tabrow
            anchors.top: parent.top
            anchors.bottom: widgetItem.visible ? widgetItem.top : parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right

            Accessible.role: Accessible.PageTabList
            orientation: Qt.Vertical
            interactive: false
            focus: true

            currentIndex: root.currentIndex
            onCurrentIndexChanged: tabrow.positionViewAtIndex(currentIndex, ListView.Contain)

            model: root.__tabs

            delegate: Item {
                implicitWidth: ListView.view.width
                implicitHeight: ListView.view.width

                Rectangle {
                    id: highlight
                    anchors.fill: parent
                    color: currentIndex == index ? "#424244" : "transparent"
                }

                Image {
                    width: 20
                    anchors.centerIn: parent
                    fillMode: Image.PreserveAspectFit
                    source: modelData.icon
                }

                BadgeItem {
                    id: badge
                    anchors.right: parent.right
                    anchors.rightMargin: 3
                    anchors.top: parent.top
                    anchors.topMargin: 6
                    count: modelData.badge
                }

                Rectangle {
                    width: parent.width
                    height: 1
                    y: parent.height - 1
                    color: "#585858"
                }

                MouseArea {
                    anchors.fill: parent
                    onPressed: {
                        __setCurrentItem(index, modelData)
                    }
                }
            }
        }

        Item {
            id: widgetItem
            width: parent.width
            height: footerLoader.height
            anchors.bottom: parent.bottom
            visible: root.visibleWidget

            Rectangle {
                width: parent.width
                height: 1
                color: "#585858"
            }

            Loader {
                id: footerLoader
                width: parent.width
                sourceComponent: root.widget
                active: root.widget
            }
        }
    }

    Loader {
        id: frameLoader
        z: t.z - 1
        anchors.left: t.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.right: parent.right

        Item {
            id: stack
            anchors.fill: parent
            Component.onCompleted: addTabs(stack.children)

            function addTabs(tabs) {
                var tabAdded = false

                for (var i = 0 ; i < tabs.length ; ++i) {
                    var tab = tabs[i]
                    if (!tab.__inserted && tab.Accessible.role === Accessible.LayeredPane) {
                        tab.__inserted = true

                        if (tab.parent === root) {
                            tab.parent = stack
                            tab.Component.onDestruction.connect(stack.onDynamicTabDestroyed.bind(tab))
                        }

                        __tabs.append({tab: tab})
                        tabAdded = true
                    }
                }

                if (tabAdded)
                    __setOpacities()
            }

            function onDynamicTabDestroyed() {
                for (var i = 0; i < stack.children.length; ++i) {
                    if (this === stack.children[i]) {
                        root.removeTab(i)
                        break
                    }
                }
            }
        }
    }

    onCurrentIndexChanged: __setOpacities()
    onChildrenChanged: stack.addTabs(root.children)
}
