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

#ifndef ATTACHMENTITEM_H
#define ATTACHMENTITEM_H

#include <QObject>
#include <QSharedPointer>
#include <QMetaEnum>
#include "notifypropertybase.h"

class Attachment : public NotifyPropertyBase
{
    Q_OBJECT
    Q_ENUMS(AttachmentType)
    Q_PROPERTY(AttachmentType type READ attachmentType) //todo

public:
    enum AttachmentType
    {
        photo,
        video,
        audio,
        doc,
        map,
        fwd_messages,
        unknown = -1
    }; //VK types in lower case. QMetaObject

    Attachment();
    AttachmentType attachmentType() const;
    int ownerId() const;
    void setOwnerId(const int ownerId);
    int uploadProgress() const;
    void setUploadProgress(const int progress);
    bool isUploading() const;
    QString toString() const;

protected:
    void setAttachmentType(const AttachmentType attachmentType);

private:
    AttachmentType _attachmentType;
    int _ownerId;
    int _uploadProgress;
};

typedef QSharedPointer<Attachment> AttachmentItem;

#endif // ATTACHMENTITEM_H