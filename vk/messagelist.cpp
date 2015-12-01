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

#include "messagelist.h"

MessageListPrivate::MessageListPrivate()
{
}

int MessageListPrivate::notFakeMsgCount() const
{
    int k = 0;

    for (int i = 0; i < count(); i++)
    {
        if (!at(i)->isFake() && !at(i)->isDeleted())
        {
            k++;
        }
    }

    return k;
}
