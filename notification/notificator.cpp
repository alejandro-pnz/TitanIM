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

#include "notificator.h"

#if defined(Q_OS_MAC)
#include "macnotification.h"
#endif

Notificator *Notificator::aInstance = 0;

Notificator *Notificator::instance()
{
    if (!aInstance) aInstance = new Notificator();
    return aInstance;
}

void Notificator::destroy()
{
    if (aInstance)
        delete aInstance, aInstance = 0;
}

Notificator::Notificator()
{
#if defined(Q_OS_MAC)
    connect(MacNotification::instance(), SIGNAL(notificationClicked(int,int)), this, SIGNAL(notificationClicked(int,int)));
    connect(MacNotification::instance(), SIGNAL(notificationReplied(int,int,QString)), this, SIGNAL(notificationReplied(int,int,QString)));
#endif
    p = new AsemanNotification();
    connect(p, SIGNAL(notifyAction(uint, QString&)), this, SLOT(notifyAction(uint, QString&)));

}

Notificator::~Notificator()
{
#if defined(Q_OS_MAC)
    MacNotification::instance()->destroy();
#endif
}

void Notificator::showNotification(const int peer, const int mid, const QString &title, const QString &message, const bool withReply, const QPixmap &pixmap)
{
#if defined(Q_OS_MAC)
    MacNotification::instance()->showNotification(peer, mid, title, message, withReply, pixmap);
#elif defined(Q_OS_WIN)
    //todo win
#else
    uint id = p->sendNotify(title, message, QString(), 0, -1);
    nl[id] = { peer, mid };
#endif
}

void Notificator::playSoundMessageIn()
{
    static QString fileName = Settings::instance()->dataDir() + "/sounds/message.wav";
    static QString cmd = Settings::instance()->loadMain("main/cmdSound", "aplay -q").toString();
    Utils::playSound(fileName, cmd);
}

void Notificator::setBadge(const int count)
{
#if defined(Q_OS_MAC)
    QtMac::setBadgeLabelText(count > 0 ? QString::number(count) : "");
#else
    //todo win and lin
#endif
}

void Notificator::notifyAction(uint id, QString &act)
{
    if (act != "default") return;
    auto it = nl.find(id);

    if (it != nl.end())
    {
        int peer = it->first;
        int mid = it->second;
        nl.erase(it);
        emit notificationClicked(peer, mid);
    }
}
