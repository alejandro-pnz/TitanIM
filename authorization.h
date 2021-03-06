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

#ifndef AUTHORIZATION_H
#define AUTHORIZATION_H

#include <QObject>
#include "vk/client.h"
#include "settings.h"

class Authorization : public QObject
{
    Q_OBJECT

public:
    Authorization();

public slots:
    void connectToVk();
    void connectToVk(const QString &username, const QString &password);
    void disconnectVk();
    void setCaptcha(const QString &sid, const QString &text);
    void cancelCaptcha();
    void setVerificationCode(const QString &sid, const QString &code);
    void checkUrlValidation(const QUrl &url);
    void showValidation(const QString &validationUri);
    void keepOnline(const bool on=true);

private slots:
    void onAuthorized(const int uid, const QString &token, const QString &secret);
    void saveSession(const int uid, const QString &token, const QString &secret);
    void onLogout(const int uid);
    void onError(const ErrorResponse::Error &error, const QString &text, const bool global, const bool fatal);

signals:
    void authorized(const int uid, const QString &token, const QString &secret);
    void logout(const int uid);
    void captcha(const QString &captchaSid, const QString &captchaImg);
    void validation(const QString &validationUri);
    void verification(const QString &validationSid, const QString &phoneMask, const bool sms, const QString &validationUri);
    void incorrectCode();
    void showAuthPage();
    void showMainPage();
};

#endif // AUTHORIZATION_H
