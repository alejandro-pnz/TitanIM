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
    id: stickerAttachments

    implicitWidth: img.width - 15
    implicitHeight: img.height - 15

    Image {
        id: img
        width: items[0].width > items[0].height ? 200 : 200 * items[0].width / items[0].height
        height: items[0].height > items[0].width ? 200 : 200 * items[0].height / items[0].width
        smooth: true
        source: items[0].src
    }
}
