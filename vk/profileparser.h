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

#ifndef PROFILEPARSER_H
#define PROFILEPARSER_H

#include <QVariantMap>
#include "client.h"
#include "profileitem.h"
#include "profilelist.h"

class ProfileItemPrivate;

class ProfileParser : public QObject
{
private:
    friend class ProfileItemPrivate;
    static void parser(const QVariantMap &item, ProfileItemPrivate *profile);

public:
    static void parser(const QVariantMap &item, ProfileItem profile);
    static ProfileItem parser(const QVariantMap &item, const bool isCached=true);
    static ProfileList parser(const QVariantList &items, const bool isCached=true);
};

#endif // PROFILEPARSER_H
