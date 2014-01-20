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

#include "errorresponse.h"

ErrorResponse::ErrorResponse(const QVariantMap &response)
{
    try
    {
        QVariantMap error;

        switch (response["error"].type()) {
        case QVariant::String:
        {
            QVariant tempError = response["error"];

            QString errorString = tempError.toString().toLower();

            if (errorString.contains("need_captcha"))
            {
                _code = CaptchaNeeded;
            }
            else if (errorString.contains("invalid_client"))
            {
                _code = UserAuthorizationFailed;
            }
            else if (errorString.contains("need_validation"))
            {
                _code = ValidationRequired;
            }
            else
            {
                _code = UnknownErrorOccured;
            }

            _msg = tempError.toString();

            error = response;
            break;
        }

        case QVariant::Map:
        {
            QVariantMap tempError = response["error"].toMap();

            _code = (Error)tempError["error_code"].toInt();
            _msg = tempError["error_msg"].toString();

            error = tempError;
            break;
        }

        default:
        {
            throw;
            break;
        }
        }

        if (_code == CaptchaNeeded)
        {
            _captchaSid = error["captcha_sid"].toString();
            _captchaImg = error["captcha_img"].toString();
        }
        else if (_code == ValidationRequired)
        {
            _validationUri = error["redirect_uri"].toString();
        }
    }
    catch(...)
    {
        _code = UnknownErrorOccured;
        _msg = "Unknown error occurred";
    }

    _global = isGlobal(_code);
    _fatal = isFatal(_code);
}

ErrorResponse::ErrorResponse(const Error &code, const QString &msg)
{
    _code = code;
    _msg = msg;
    _global = isGlobal(_code);
    _fatal = isFatal(_code);
}

Error ErrorResponse::code() const
{
    return _code;
}

QString ErrorResponse::msg() const
{
    return _msg;
}

bool ErrorResponse::global() const
{
    return _global;
}

bool ErrorResponse::fatal() const
{
    return _fatal;
}

QString ErrorResponse::captchaSid() const
{
    return _captchaSid;
}

QString ErrorResponse::captchaImg() const
{
    return _captchaImg;
}

QString ErrorResponse::validationUri() const
{
    return _validationUri;
}

bool ErrorResponse::isGlobal(const Error &code)
{
    if (code == LoadTokenFailed ||
        code == TimeoutLongPollServer ||
        code == ServerIsNotAvailable ||
        code == UnknownErrorOccured ||
        code == ApplicationIsDisabled ||
        code == UnknownMethodPassed ||
        code == IncorrectSignature ||
        code == UserAuthorizationFailed ||
        code == TooManyRequestsPerSecond ||
        code == CaptchaNeeded ||
        code == HttpAuthorizationFailed ||
        code == ValidationRequired)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool ErrorResponse::isFatal(const Error &code)
{
    if (code == LoadTokenFailed ||
        code == ApplicationIsDisabled ||
        code == UserAuthorizationFailed ||
        code == ValidationRequired)
    {
        return true;
    }
    else
    {
        return false;
    }
}
