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

#ifndef CHATMODEL_H
#define CHATMODEL_H

#include <QAbstractListModel>
#include <QStringList>
#include "vk/messagelist.h"
#include "vk/dialogitem.h"
#include "vk/utils.h"

class ChatModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum chatRole
    {
        bodyRole = Qt::UserRole,
        dateRole,
        dateStrRole,
        uidRole,
        midRole,
        isUnreadRole,
        isOutRole
    };

    explicit ChatModel(const DialogItem dialog, const ProfileItem ownProfile, QObject *parent = 0);
    ~ChatModel();
    void append(const MessageList items);
    void append(const MessageItem item, const bool replace=false);
    void prepend(const MessageItem item, const bool replace=false);
    void replaceAll(const MessageList items);
    bool remove(int row, int count);
    MessageItem at(const int row) const;
    int indexOf(const int mid) const;
    QHash<int, QByteArray> roleNames() const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant& value, int role = Qt::EditRole);
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;

private:
    MessageList _messages;
    DialogItem _dialog;
    ProfileItem _ownProfile;

protected slots:
    void onItemChanged(const int i);
    void onRowsChanged(const QModelIndex &parent, int first, int last);
};

#endif // CHATMODEL_H
