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

#include "chatshandler.h"

ChatsHandler::ChatsHandler()
{
    LongPoll *longPoll = Client::instance()->longPoll();

    connect(longPoll, &LongPoll::messageInAdded, this, &ChatsHandler::onLongPollMessageInAdded);
    connect(longPoll, &LongPoll::messageOutAdded, this, &ChatsHandler::onLongPollMessageOutAdded);
    connect(longPoll, &LongPoll::messageFlagsSet, this, &ChatsHandler::onMessageFlagsSet);
    connect(longPoll, &LongPoll::messageFlagsReseted, this, &ChatsHandler::onMessageFlagsReseted);
    connect(longPoll, &LongPoll::obsoleteFriendsOnline, this, &ChatsHandler::onObsoleteFriendsOnline);
    connect(longPoll, &LongPoll::rebuild, this, &ChatsHandler::onRebuild);
}

ChatsHandler::~ChatsHandler()
{
    clear();
}

void ChatsHandler::create(const DialogItem dialog)
{
    int id = dialog->id();

    if (!_chats.contains(id))
    {
        Chat *chat = new Chat(dialog);
        _chats[id] = chat;
        _ids.enqueue(id);

        chat->model()->load();

        if (_ids.count() > 10)
        {
            int key = _ids.dequeue();
            delete _chats.value(key);
            _chats.remove(key);
        }
    }
}

bool ChatsHandler::contains(const int id) const
{
    return _chats.contains(id);
}

Chat *ChatsHandler::chat(const int id) const
{
    return _chats.value(id);
}

void ChatsHandler::clear()
{
    QMapIterator<int, Chat*> i(_chats);
    while (i.hasNext()) {
        i.next();
        delete i.value();
    }

    _chats.clear();
    _ids.clear();
}

void ChatsHandler::onLongPollMessageInAdded(const int id, const MessageItem message, const ProfileItem profile)
{
    Q_UNUSED(profile);

    if (contains(id))
    {
        if (message->actionMid() > 0)
        {
            message->getAllFields(Client::instance()->connection());
        }

        chat(id)->addInMessage(message);
    }
}

void ChatsHandler::onLongPollMessageOutAdded(const int id, const MessageItem message, const ProfileItem profile)
{
    Q_UNUSED(profile);

    if (contains(id))
    {
        if (message->actionMid() > 0)
        {
            message->getAllFields(Client::instance()->connection());
        }

        Chat *chat = this->chat(id);

        if (chat->countUnsent() > 0)
        {
            chat->addTempOutMessageQueue(message);
        }
        else
        {
            chat->addOutMessage(message);
        }
    }
}

void ChatsHandler::onMessageFlagsSet(const int mid, const int mask, const int id)
{
    if (contains(id))
    {
        if ((mask & LongPoll::Deleted) || (mask & LongPoll::Spam))
        {
            chat(id)->model()->markAsDeleted(mid, true);
        }
    }
}

void ChatsHandler::onMessageFlagsReseted(const int mid, const int mask, const int id, const uint date)
{
    if (contains(id))
    {
        if (mask & LongPoll::Unread)
        {
            chat(id)->model()->markAsRead(mid);
        }

        if ((mask & LongPoll::Deleted) || (mask & LongPoll::Spam))
        {
            if (!chat(id)->model()->markAsDeleted(mid, false))
            {
                //recovery message
                MessageItem message = MessageItem::create();
                message->setId(mid);
                message->setFromId(id);
                message->setDate(QDateTime::fromTime_t(date).toLocalTime());
                message->setDeleted(true); //not bug
                message->getAllFields(Client::instance()->connection());

                chat(id)->model()->append(message, true);
            }
        }
    }
}

void ChatsHandler::onObsoleteFriendsOnline()
{
    QMapIterator<int, Chat*> i(_chats);
    while (i.hasNext()) {
        i.next();
        Chat* _chat = i.value();

        if (_chat->isCurrent())
        {
            _chat->dialog()->profile()->getLastActivity(Client::instance()->connection());
        }
    }
}

void ChatsHandler::onRebuild()
{
    QMapIterator<int, Chat*> i(_chats);
    while (i.hasNext()) {
        i.next();

        int _key = i.key();
        Chat* _chat = i.value();

        if (_chat->isCurrent() || _chat->isBusy())
        {
            _chat->refreshHistory();
        }
        else
        {
            _chats.remove(_key);
            delete _chat;
            _ids.removeOne(_key);
        }
    }
}
