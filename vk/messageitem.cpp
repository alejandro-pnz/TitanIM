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

#include "messageitem.h"

MessageItemPrivate::MessageItemPrivate()
{
    _uid = -1;
    _isError = false;
    _chatId = -1;
}

MessageItemPrivate::~MessageItemPrivate()
{
}

int MessageItemPrivate::mid() const
{
    return _mid;
}

void MessageItemPrivate::setMid(const int mid)
{
    if (_mid != mid)
    {
        _mid = mid;
        emit propertyChanged(_mid, "mid");
    }
}

int MessageItemPrivate::uid() const
{
    return _uid;
}

void MessageItemPrivate::setUid(const int uid)
{
    if (_uid != uid)
    {
        _uid = uid;
        emit propertyChanged(_mid, "uid");
    }
}

QDateTime MessageItemPrivate::date() const
{
    return _date;
}

void MessageItemPrivate::setDate(const QDateTime &date)
{
    if (_date != date)
    {
        _date = date;
        emit propertyChanged(_mid, "date");
    }
}

bool MessageItemPrivate::isUnread() const
{
    return _isUnread;
}

void MessageItemPrivate::setIsUnread(const bool isUnread)
{
    if (_isUnread != isUnread)
    {
        _isUnread = isUnread;
        emit propertyChanged(_mid, "isUnread");
    }
}

bool MessageItemPrivate::isOut() const
{
    return _isOut;
}

void MessageItemPrivate::setIsOut(const bool isOut)
{
    if (_isOut != isOut)
    {
        _isOut = isOut;
        emit propertyChanged(_mid, "isOut");
    }
}

bool MessageItemPrivate::isError() const
{
    return _isError;
}

void MessageItemPrivate::setIsError(const bool isError)
{
    if (_isError != isError)
    {
        _isError = isError;
        emit propertyChanged(_mid, "isError");
    }
}

bool MessageItemPrivate::isSending() const
{
    return _mid < 0;
}

bool MessageItemPrivate::deliveryReport() const
{
    return _deliveryReport;
}

void MessageItemPrivate::setDeliveryReport(const bool deliveryReport)
{
    if (_deliveryReport != deliveryReport)
    {
        _deliveryReport = deliveryReport;
        emit propertyChanged(_mid, "deliveryReport");
    }
}

QString MessageItemPrivate::title() const
{
    return _title;
}

void MessageItemPrivate::setTitle(const QString &title)
{
    if (_title != title)
    {
        _title = title;
        emit propertyChanged(_mid, "title");
    }
}

QString MessageItemPrivate::body() const
{
    return _body;
}

void MessageItemPrivate::setBody(const QString &body)
{
    if (_body != body)
    {
        _body = body;
        emit propertyChanged(_mid, "body");
    }
}

int MessageItemPrivate::chatId() const
{
    return _chatId;
}

void MessageItemPrivate::setChatId(const int chatId)
{
    if (_chatId != chatId)
    {
        _chatId = chatId;
        emit propertyChanged(_mid, "chatId");
    }
}

bool MessageItemPrivate::isGroupChat() const
{
    return _chatId == -1 ? false : true;
}
