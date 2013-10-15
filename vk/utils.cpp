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

#include "utils.h"

QVariant Utils::parseJSON(const QByteArray &data)
{
    QVariant res;
    int len = data.size();
    K8JSON::parseRecord(res, reinterpret_cast<const uchar *>(data.constData()), &len);
    return res;
}

QString Utils::getMd5String(const QString &source)
{
    return QString(QCryptographicHash::hash(source.toUtf8(), QCryptographicHash::Md5).toHex());
}

QString Utils::join(const QMap<QString, QString> &list, const QChar &separator)
{
    QString s;
    QMapIterator<QString, QString> i(list);
    while (i.hasNext())
    {
        i.next();
        s.append(i.key() % "=" % i.value() % separator);
    }

    s.resize(s.length() - 1);
    return s;
}

QString Utils::joinAndEscape(const QMap<QString, QString> &list, const QChar &separator)
{
    QString s;
    QMapIterator<QString, QString> i(list);
    while (i.hasNext())
    {
        i.next();
        s.append(i.key() % "=" % QUrl::toPercentEncoding(i.value()) % separator);
    }

    s.resize(s.length() - 1);
    return s;
}

QString Utils::decode(const QString &str)
{
    QString temp = str;
    temp = temp.replace("<br>", "\n");
    temp = temp.replace("&amp;", "&");
    temp = temp.replace("&lt;", "<");
    temp = temp.replace("&gt;", ">");
    temp = temp.replace("&quot;", "\"");
    temp = temp.replace("&#33;", "!");
    temp = temp.replace("&#036;", "$");
    temp = temp.replace("&#092;", "\\");
    temp = temp.replace("&#39;", "'");

    return temp;
}

QString Utils::photoUrlFix(const QString &url)
{
    return url.endsWith(".gif") ? "Images/camera_b.png" : url;
}
