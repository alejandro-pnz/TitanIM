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

#ifndef UPLOADFILE_H
#define UPLOADFILE_H

#include <QObject>
#include <QFile>
#include <QFileInfo>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QHttpMultiPart>
#include <QMimeDatabase>

class UploadFile : public QObject
{
    Q_OBJECT

public:
    explicit UploadFile(QObject *parent = 0);
    int id() const;
    void setId(const int id);
    void upload(const QUrl &url, const QString &fileName, const QString &field);

private:
    int _id;
    QNetworkAccessManager *_manager;

protected slots:
    void uploadFinished(QNetworkReply *networkReply);

signals:
    void finished(const int id, const QByteArray &result);
};

#endif // UPLOADFILE_H
