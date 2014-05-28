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

#include "typingitem.h"

TypingItemPrivate::TypingItemPrivate()
{
    setMessageType(Typing);

    _uid = 0;
    _date = QDateTime::currentDateTime();

    _timer = new QTimer(this);
    _timer->setSingleShot(true);
    _timer->setInterval(8000);

    connect(_timer, SIGNAL(timeout()), this, SLOT(onTimerTimeout()));

    _timer->start();
}

int TypingItemPrivate::uid() const
{
    return _uid;
}

void TypingItemPrivate::setUid(const int uid)
{
    if (_uid != uid)
    {
        _uid = uid;
        emitPropertyChanged("uid");
    }
}

void TypingItemPrivate::restart()
{
    if (!isActive())
    {
        setDate(QDateTime::currentDateTime());
        setIsActive(true);
    }

    _timer->start();
}

void TypingItemPrivate::stop()
{
    _timer->stop();
    setIsActive(false);
}

void TypingItemPrivate::setIsActive(const bool isActive)
{
    if (this->isActive() != isActive)
    {
        setDeleted(!isActive);
        emit activeChanged(_id, _uid, isActive);
    }
}

void TypingItemPrivate::onTimerTimeout()
{
    setIsActive(false);
}
