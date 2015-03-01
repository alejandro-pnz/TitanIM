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

#ifndef ENGINE_H
#define ENGINE_H

#include <QObject>
#include <QQmlEngine>
#include <QHash>
#include "connection.h"
#include "profileitem.h"

class Engine : public QObject
{
    Q_OBJECT

public:
    Engine(Connection *connection);
    ~Engine();

private:
    Connection *_connection;
    ProfileItem _selfProfile;
    QHash<int, ProfileItem> *_profiles;

public slots:
    int uid() const;
    ProfileItem getProfile() const;
    QVariant getUser();
    ProfileItem getProfile(const int id, ProfileItem defaultValue=ProfileItem());
    QVariant getUser(const int id);

private slots:
    void onConnected(const int uid, const QString &token, const QString &secret);
    void onDisconnected();
};

#endif // ENGINE_H