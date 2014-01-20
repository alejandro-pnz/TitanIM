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

#include "notifypropertybase.h"

NotifyPropertyBase::NotifyPropertyBase()
{
    _id = 0;
}

void NotifyPropertyBase::setId(const int id)
{
    if (_id != id)
    {
        _id = id;
        emit propertyChanged(_id, "id");
    }
}
