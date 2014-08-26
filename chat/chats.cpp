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

#include "chats.h"

Chats *Chats::aInstance = 0;

Chats *Chats::instance()
{
    if (!aInstance) aInstance = new Chats();
    return aInstance;
}

void Chats::destroy()
{
    if (aInstance)
        delete aInstance, aInstance = 0;
}

Chats::Chats()
{
    _chatsHandler = new ChatsHandler();
    _proxy = new ChatSortFilterProxyModel(this);

    _currentChatId = 0;
    _currentDialog = 0;

    _timerUpdater = new QTimer(this);
    connect(_timerUpdater, SIGNAL(timeout()), this, SLOT(onTimerUpdaterTimeout()));
    _timerUpdater->start(60000);

    qmlRegisterType<Chat>("TitanIM", 2, 0, "Chat");
    qmlRegisterType<DialogItemPrivate>("TitanIM", 2, 0, "DialogItem");
    qRegisterMetaType<ChatSortFilterProxyModel*>("ChatSortFilterProxyModel*");
    qRegisterMetaType<QSortFilterProxyModel*>("QSortFilterProxyModel*");
    qmlRegisterType<AttachmentList>("TitanIM", 2, 0, "AttachmentList");
    qmlRegisterType<Attachment>("TitanIM", 2, 0, "Attachment");
    qRegisterMetaType<Attachment::AttachmentType>("Attachment::AttachmentType");
    qmlRegisterType<MessageBase>("TitanIM", 2, 0, "MessageBase");
    qRegisterMetaType<MessageBase::MessageType>("MessageBase::MessageType");
}

Chats::~Chats()
{
    delete _chatsHandler;
}

int Chats::currentChatId() const
{
    return _currentChatId;
}

Chat *Chats::currentChat() const
{
    return _chatsHandler->chat(_currentChatId);
}

DialogItemPrivate *Chats::currentChatDialog() const
{
    return _currentDialog;
}

ChatSortFilterProxyModel* Chats::currentChatModel() const
{
    return _proxy;
}

void Chats::setCurrentChat(const int id)
{
    if (_currentChatId != id)
    {
        if (_currentDialog)
        {
            _currentDialog->setCurrent(false);
        }

        Chat *chat = _chatsHandler->chat(id);

        chat->model()->setLazyLoad(false);
        _proxy->setSourceModel(chat->model());
        chat->model()->setLazyLoad(true);

        _currentDialog = chat->dialog().data();
        _currentDialog->setCurrent(true);
        _currentChatId = id;

        chat->markAsRead();

        emit currentChatChanged(id);
    }
}

void Chats::openChat(const DialogItem dialog)
{
    int id = dialog->id();

    if (!_chatsHandler->contains(id))
    {
        _chatsHandler->create(dialog);
    }

    setCurrentChat(id);
}

void Chats::onTimerUpdaterTimeout()
{
    if (_currentDialog)
    {
        if (_currentDialog->isGroupChat())
        {
            _currentDialog->groupChatHandler()->updatePeopleConversationText();
        }
        else
        {
//            if (_currentDialog->profile()->isFriend()) //todo
//            {
                _currentDialog->profile()->updateLastSeenText();
//            }
//            else
//            {
//                _currentDialog->profile()->getAllFields();
//            }
        }
    }
}
