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

#ifndef SENDMESSAGEHANDLER_H
#define SENDMESSAGEHANDLER_H

#include <QObject>
#include <QQueue>
#include "connection.h"
#include "messageitem.h"

class SendMessageHandler : public QObject
{
    Q_OBJECT

public:
    explicit SendMessageHandler(Connection *connection);
    void send(MessageItem message);

private:
    Connection *_connection;
    QQueue<MessageItem> _messageQuery;
    bool _isProcessing;
    QMap<int,MessageItem> _messagesInProcessing;

private:
    void execSendMessageQuery();
    void sendMessage();

private slots:
    void sendMessageFinished(const Packet *sender, const QVariantMap &result);

signals:
    void sending(const int internalMid);
    void successfullyMessageSent(const int internalMid, const int serverMid);
};

#endif // SENDMESSAGEHANDLER_H
