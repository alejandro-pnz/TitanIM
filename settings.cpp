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

#include "settings.h"

Settings *Settings::aInstance = 0;

Settings *Settings::instance()
{
    if (!aInstance) aInstance = new Settings();
    return aInstance;
}

void Settings::destroy()
{
    if (aInstance)
        delete aInstance, aInstance = 0;
}

Settings::Settings()
{
    if (QFile::exists(QCoreApplication::applicationDirPath() + "/settings.ini"))
    {
        _configDir = QCoreApplication::applicationDirPath();
    }
    else
    {
        _configDir = QString("%1/%2/%3").arg(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation),
                                             QCoreApplication::organizationName(),
                                             QCoreApplication::applicationName());
    }

    #if defined(Q_OS_MAC)
    _dataDir = QCoreApplication::applicationDirPath() + "/../Resources/data";
    #elif defined(Q_OS_UNIX)
    _dataDir = QString("/opt/%1/data").arg(QCoreApplication::applicationName().toLower());
    #elif defined(Q_OS_WIN)
    _dataDir = QCoreApplication::applicationDirPath() + "/data";
    #else
    _dataDir = "";
    qWarning() << "Warning: Unknown OS. Data dir not defined";
    #endif
}

QStringList Settings::uids()
{
    QDir dir(_configDir + "/profiles");
    QStringList uids = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

    QString last = lastUid();

    if (uids.contains(last))
    {
        uids.swap(uids.indexOf(last), 0);
    }

    return uids;
}

void Settings::setCurrentUid(const QString &uid)
{
    _currentUid = uid;
    createDirs();
}

QString Settings::lastUid()
{
    return loadMain("profiles/last", "").toString();
}

void Settings::createDirs()
{
    QDir dir;

    if (!dir.exists(profileDir()))
        dir.mkpath(profileDir());

    if (!dir.exists(cacheDir()))
        dir.mkpath(cacheDir());

    if (!dir.exists(_configDir + "/dictionaries"))
        dir.mkpath(_configDir + "/dictionaries");
}

QString Settings::configDir() const
{
    return _configDir;
}

QString Settings::dataDir() const
{
    return _dataDir;
}

QString Settings::profileDir(const QString &uid) const
{
    if (uid.isEmpty())
    {
        return _configDir + "/profiles/" + _currentUid;
    }
    else
    {
        return _configDir + "/profiles/" + uid;
    }
}

QString Settings::cacheDir() const
{
    return _configDir + "/cache";
}

void Settings::saveProfile(const QString &key, const QVariant &value, const QString &uid)
{
    QSettings settings(profileDir(uid) + "/profilesettings.ini", QSettings::IniFormat);
    settings.setValue(key, value);
}

void Settings::saveMain(const QString &key, const QVariant &value)
{
    QSettings settingsMain(configDir() + "/settings.ini", QSettings::IniFormat);
    settingsMain.setValue(key, value);
}

QVariant Settings::loadProfile(const QString &key, const QVariant &defaultValue, const QString &uid)
{
    QSettings settings(profileDir(uid) + "/profilesettings.ini", QSettings::IniFormat);
    return settings.value(key, defaultValue);
}

bool Settings::loadBoolProfile(const QString &key, const bool &defaultValue, const QString &uid)
{
    return loadProfile(key, defaultValue, uid).toBool();
}

QVariant Settings::loadMain(const QString &key, const QVariant &defaultValue)
{
    QSettings settingsMain(configDir() + "/settings.ini", QSettings::IniFormat);
    return settingsMain.value(key, defaultValue);
}

bool Settings::loadBoolMain(const QString &key, const bool &defaultValue)
{
    return loadMain(key, defaultValue).toBool();
}
