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

#include "dialogitem.h"

DialogItemPrivate::DialogItemPrivate()
{
    _groupChatHandler = 0;

    connect(this, SIGNAL(propertyChanged(int,QString)), this, SIGNAL(displayNameChanged())); //todo
    connect(this, SIGNAL(propertyChanged(int,QString)), this, SIGNAL(decorationChanged())); //todo
}

DialogItemPrivate::~DialogItemPrivate()
{
    if (_groupChatHandler)
    {
        delete _groupChatHandler;
    }
}

QString DialogItemPrivate::displayName() const
{
    return isGroupChat() ? _groupChatHandler->title() : _profile->fullName();
}

QStringList DialogItemPrivate::decoration() const
{
    return isGroupChat() ? _groupChatHandler->avatars() : QStringList(_profile->photoMediumRect());
}

bool DialogItemPrivate::isGroupChat() const
{
    return _groupChatHandler;
}

ProfileItem DialogItemPrivate::profile() const
{
    return _profile;
}

void DialogItemPrivate::setProfile(const ProfileItem profile)
{
    if (_profile.data() != profile.data())
    {
        if (_profile)
        {
            disconnect(_profile.data(), SIGNAL(propertyChanged(int,QString)), this, SLOT(onProfilePropertyChanged(int,QString)));
        }

        _profile = profile;

        if (!_groupChatHandler)
        {
            _id = _profile->id();
        }

        connect(profile.data(), SIGNAL(propertyChanged(int,QString)), this, SLOT(onProfilePropertyChanged(int,QString)));
        emitPropertyChanged("profile");
    }
}

MessageItem DialogItemPrivate::message() const
{
    return _message;
}

void DialogItemPrivate::setMessage(const MessageItem message)
{
    if (_message.data() != message.data())
    {
        if (_message)
        {
            disconnect(_message.data(), SIGNAL(propertyChanged(int,QString)), this, SLOT(onMessagePropertyChanged(int,QString)));
        }

        _message = message;

        if (!_profile && !_groupChatHandler)
        {
            _id = _message->uid();
        }

        connect(message.data(), SIGNAL(propertyChanged(int,QString)), this, SLOT(onMessagePropertyChanged(int,QString)));
        emitPropertyChanged("message");
    }
}

GroupChatHandler *DialogItemPrivate::groupChatHandler() const
{
    return _groupChatHandler;
}

void DialogItemPrivate::setGroupChatHandler(GroupChatHandler *groupChatHandler)
{
    if (_groupChatHandler != groupChatHandler)
    {
        if (_groupChatHandler)
        {
            disconnect(_groupChatHandler, SIGNAL(propertyChanged(int,QString)), this, SLOT(onGroupChatPropertyChanged(int,QString)));
            delete _groupChatHandler;
        }

        _groupChatHandler = groupChatHandler;

        _id = groupChatHandler->id();

        connect(_groupChatHandler, SIGNAL(propertyChanged(int,QString)), this, SLOT(onGroupChatPropertyChanged(int,QString)));
        emitPropertyChanged("groupChat");
    }
}

void DialogItemPrivate::onProfilePropertyChanged(const int uid, const QString &propertyName)
{
    QString property = QString("profile.%1").arg(propertyName);
    emitPropertyChanged(property);
}

void DialogItemPrivate::onMessagePropertyChanged(const int mid, const QString &propertyName)
{
    QString property = QString("message.%1").arg(propertyName);
    emitPropertyChanged(property);
}

void DialogItemPrivate::onGroupChatPropertyChanged(const int chatId, const QString &propertyName)
{
    QString property = QString("groupChat.%1").arg(propertyName);
    emitPropertyChanged(property);
}