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

#ifndef PHOTOPARSER_H
#define PHOTOPARSER_H

#include <QObject>
#include <QVariantMap>
#include "photoitem.h"

class PhotoParser : public QObject
{
public:
    static PhotoItem parser(const QVariantMap &item);
};

#endif // PHOTOPARSER_H
