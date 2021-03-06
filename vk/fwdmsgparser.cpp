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

#include "fwdmsgparser.h"

FwdMsgItem FwdMsgParser::parser(const QVariantList &items)
{
    FwdMsgItem fwdMsg = FwdMsgItem::create();

    fwdMsg->setMessages(MessageParser::parser(items));

    return fwdMsg;
}
