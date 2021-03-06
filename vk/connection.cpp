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

#include "connection.h"

Connection::Connection(const QString &clientId, const QString &clientSecret)
{
    _loginVars.grant_type = "password";
    _loginVars.client_id = clientId;
    _loginVars.client_secret = clientSecret;
    _loginVars.scope = "friends,photos,audio,video,docs,notes,status,wall,groups,messages,notifications";

    _urlServers.auth_server = "https://oauth.vk.com/token";
    _urlServers.api_server = "https://api.vk.com";

    _isHttps = true;
    _isProcessing = false;
    _isAuthorized = false;

    httpApi = 0;

    bearer = new QNetworkConfigurationManager(this);
    connect(bearer, &QNetworkConfigurationManager::onlineStateChanged, this, &Connection::onNetworkOnlineChanged);

    tooManyRequestsTimer = new QTimer();
    tooManyRequestsTimer->setInterval(700);
    connect(tooManyRequestsTimer, &QTimer::timeout, this, &Connection::onTooManyRequestsTimerTick);
}

Connection::~Connection()
{
    delete tooManyRequestsTimer;
}

void Connection::connectToVk(const QString &username, const QString &password, const bool https)
{
    if (isAuthorized() || username.isEmpty() || password.isEmpty())
    {
        return;
    }

    setHttpsMode(https);

    httpApi = new QNetworkAccessManager(this);
    connect(httpApi, &QNetworkAccessManager::finished, this, &Connection::apiResponse);

    _loginVars.username = username;
    _loginVars.password = password;

    getToken();
}

void Connection::connectToVk(const int uid, const QString &token, const QString &secret)
{
    if (isAuthorized())
    {
        return;
    }

    httpApi = new QNetworkAccessManager(this);
    connect(httpApi, &QNetworkAccessManager::finished, this, &Connection::apiResponse);

    _sessionVars.user_id = uid;
    _sessionVars.access_token = token;
    _sessionVars.secret = secret;

    setHttpsMode(_sessionVars.secret.isEmpty());

    checkToken();
}

void Connection::disconnectVk()
{
    int uid = _sessionVars.user_id;

    disconnect(httpApi, &QNetworkAccessManager::finished, this, &Connection::apiResponse);

    if (httpApi)
    {
        httpApi->deleteLater();
        httpApi = 0;
    }

    _queryList.clear();
    _sessionVars.user_id = 0;
    _sessionVars.access_token.clear();
    _sessionVars.secret.clear();
    _sessionVars.expires_in = 0;
    _isProcessing = false;
    _isAuthorized = false;

    emit logout(uid);
}

bool Connection::isAuthorized() const
{
    return _isAuthorized;
}

bool Connection::isHttps() const
{
    return _isHttps;
}

void Connection::setHttpsMode(const bool isHttps)
{
    if (_isHttps != isHttps)
    {
        _isHttps = isHttps;
        onHttpsModeChanged(_isHttps);
    }
}

void Connection::getToken(const QString &version)
{
    httpAuth = new QNetworkAccessManager(this);
    connect(httpAuth, &QNetworkAccessManager::finished, this, &Connection::getTokenFinished);

    QUrlQuery urlQuery;
    urlQuery.addQueryItem("grant_type", _loginVars.grant_type);
    urlQuery.addQueryItem("client_id", _loginVars.client_id);
    urlQuery.addQueryItem("client_secret", _loginVars.client_secret);
    urlQuery.addQueryItem("scope", _loginVars.scope);
    urlQuery.addQueryItem("username", _loginVars.username);
    urlQuery.addQueryItem("password", _loginVars.password);
    urlQuery.addQueryItem("v", version);

    if (!_loginVars.captcha_sid.isEmpty())
    {
        urlQuery.addQueryItem("captcha_sid", _loginVars.captcha_sid);
        urlQuery.addQueryItem("captcha_key", _loginVars.captcha_key);

        _loginVars.captcha_sid.clear();
        _loginVars.captcha_key.clear();
    }

    if (!_loginVars.sid.isEmpty())
    {
        urlQuery.addQueryItem("sid", _loginVars.sid);
        urlQuery.addQueryItem("code", _loginVars.code);

        _loginVars.sid.clear();
        _loginVars.code.clear();
    }

    QUrl authUrl(_urlServers.auth_server);
    authUrl.setQuery(urlQuery);

    QNetworkRequest request;
    request.setUrl(authUrl);
    httpAuth->get(request);
}

void Connection::checkToken()
{
    if (_sessionVars.access_token.isEmpty())
    {
        onError(ErrorResponse::LoadTokenFailed, "Token empty");
        return;
    }

    _isProcessing = false;

    Packet *packet = new Packet("execute.magic");
    connect(packet, &Packet::finished, this, &Connection::successfullyToken);
    connect(packet, &Packet::error, this, &Connection::unsuccessfullyToken);
    appendQuery(packet);
}

void Connection::getTokenFinished(QNetworkReply *networkReply)
{
    if (networkReply->error() != QNetworkReply::NoError)
    {
        if (networkReply->error() == QNetworkReply::AuthenticationRequiredError)
        {
            QVariantMap error = Utils::parseJSON(networkReply->readAll()).toMap();
            networkReply->deleteLater();
            httpAuth->deleteLater();

            ErrorResponse *errorResponse = new ErrorResponse(error);
            onError(errorResponse);
        }
        else
        {
            networkReply->deleteLater();
            httpAuth->deleteLater();
            onError(ErrorResponse::ServerIsNotAvailable, "Server is not available");
        }

        return;
    }

    QVariantMap response = Utils::parseJSON(networkReply->readAll()).toMap();
    networkReply->deleteLater();

    if (response.contains("https_required"))
    {
        httpAuth->deleteLater();
        onError(ErrorResponse::HttpAuthorizationFailed, "HTTP authorization failed");
        return;
    }

    if (response.contains("access_token"))
    {
        _sessionVars.access_token = response.value("access_token").toString();
        _sessionVars.user_id = response.value("user_id").toInt();
        _sessionVars.expires_in = response.value("expires_in").toInt();
        _sessionVars.secret = response.contains("secret") ? response.value("secret").toString() : "";

        checkToken();
    }
    else
    {
        onError(ErrorResponse::LoadTokenFailed, "Access_token not found");
    }

    httpAuth->deleteLater();
}

void Connection::successfullyToken(const Packet *sender, const QVariantMap &result)
{
    QVariantMap response = result.value("response").toMap();

    uint unixtime = response.value("time").toUInt();
    QDateTime dateTime = QDateTime::fromTime_t(unixtime).toLocalTime();
    Utils::setServerDateTime(dateTime);

    if (!_isAuthorized)
    {
        _isAuthorized = true;
        emit authorized(_sessionVars.user_id, _sessionVars.access_token, _sessionVars.secret);
    }
    else
    {
        emit sessionChanged(_sessionVars.user_id, _sessionVars.access_token, _sessionVars.secret);
    }

    delete sender;
}

void Connection::unsuccessfullyToken(const Packet *sender, const ErrorResponse *errorResponse)
{
    onError(ErrorResponse::LoadTokenFailed, "Check token - failed");
    delete sender;
}

void Connection::appendQuery(Packet *packet)
{
    if (_sessionVars.access_token.isEmpty())
    {
        return;
    }

    if (bearer->isOnline() || !packet->isPerishable())
    {
        _queryList.enqueue(packet);
    }
    else
    {
        packet->setError(ErrorResponse::ServerIsNotAvailable, "Server is not available");
    }

    if (!_isProcessing)
    {
        execQuery();
    }
}

void Connection::execQuery()
{
    if (_queryList.isEmpty() || _isProcessing || !bearer->isOnline())
    {
        return;
    }

    _isProcessing = true;

    Packet *currentPacket = _queryList.head();
    currentPacket->addParam("access_token", _sessionVars.access_token);
    currentPacket->signPacket(_sessionVars.secret);

    if (_isHttps)
    {
        currentPacket->addParam("https", 1);
    }

    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    request.setUrl(QUrl(_urlServers.api_server + currentPacket->urlPath()));
    httpApi->post(request, currentPacket->urlQuery());
}

void Connection::apiResponse(QNetworkReply *networkReply)
{
    if (_queryList.isEmpty())
    {
        _isProcessing = false;
        networkReply->deleteLater();
        return;
    }

    if (networkReply->error() != QNetworkReply::NoError)
    {
        networkReply->deleteLater();
        _isProcessing = false;

        if (!_queryList.isEmpty())
        {
            execQuery();
        }

        return;
    }

    QVariantMap response = Utils::parseJSON(networkReply->readAll()).toMap();
    networkReply->deleteLater();

    if (response.contains("error"))
    {
        ErrorResponse *errorResponse = new ErrorResponse(response);

        if (errorResponse->global())
        {
            //error global. ExecQuery stop
            onError(errorResponse);
            return;
        }
        else
        {
            //error local
            _queryList.dequeue()->setError(errorResponse);
        }
    }
    else
    {
        _queryList.dequeue()->setResult(response);
    }

    _isProcessing = false;

    if (!_queryList.isEmpty())
    {
        execQuery();
    }
}

void Connection::setCaptcha(const QString &sid, const QString &text)
{
    if (!_queryList.isEmpty())
    {
        //captcha in packet
        _queryList.head()->addParam("captcha_sid", sid);
        _queryList.head()->addParam("captcha_key", text);
        _isProcessing = false;
        execQuery();
    }
    else if (!isAuthorized())
    {
        //captcha in authorization
        _loginVars.captcha_sid = sid;
        _loginVars.captcha_key = text;
        getToken();
    }
}

void Connection::cancelCaptcha()
{
    clearAllQuery(ErrorResponse::CaptchaCanceled, "Captcha canceled");
}

void Connection::setVerificationCode(const QString &sid, const QString &code)
{
    _loginVars.sid = sid;
    _loginVars.code = code;
    getToken();
}

void Connection::clearAllQuery(const ErrorResponse::Error &code, const QString &msg)
{
    while (!_queryList.isEmpty())
    {
        ErrorResponse *errorResponse = new ErrorResponse(code, msg);
        _queryList.dequeue()->setError(errorResponse);
    }

    _isProcessing = false;
}

void Connection::clearPerishableQuery(const ErrorResponse::Error &code, const QString &msg)
{
    for (int i = 0; i < _queryList.count(); i++)
    {
        Packet *packet = _queryList.at(i);

        if (packet->isPerishable())
        {
            packet->setError(code, msg);
            _queryList.removeAt(i);

            i--;
        }
    }
}

void Connection::sendVerificationSms(const QString &sid)
{
    //todo request private method vk api
}

SessionVars Connection::session() const
{
    return _sessionVars;
}

void Connection::setSession(const SessionVars session)
{
    if (!session.access_token.isEmpty())
    {
        _sessionVars.access_token = session.access_token;
        _sessionVars.user_id = session.user_id;
        _sessionVars.expires_in = session.expires_in;
        _sessionVars.secret = session.secret;

        checkToken();
    }
    else
    {
        onError(ErrorResponse::LoadTokenFailed, "Access_token not found");
    }
}

void Connection::onTooManyRequestsTimerTick()
{
    tooManyRequestsTimer->stop();
    _isProcessing = false;
    if (!_queryList.isEmpty())
    {
        execQuery();
    }
}

void Connection::onNetworkOnlineChanged(const bool isOnline)
{
    static bool _isOnline = !isOnline;

    if (_isOnline != isOnline)
    {
        _isOnline = isOnline;

        if (isAuthorized())
        {
            emit networkOnlineChanged(isOnline);
        }

        if (!_queryList.isEmpty())
        {
            if (isOnline)
            {
                execQuery();
            }
            else
            {
                clearPerishableQuery(ErrorResponse::ServerIsNotAvailable, "Server is not available");
            }
        }
    }
}

void Connection::onHttpsModeChanged(const bool isHttps)
{
    if (isHttps)
    {
        _loginVars.scope.remove(",nohttps");
        _urlServers.api_server.replace("http://", "https://");
    }
    else
    {
        _loginVars.scope += ",nohttps";
        _urlServers.api_server.replace("https://", "http://");
    }
}

void Connection::onError(const ErrorResponse *errorResponse)
{
    if (errorResponse->fatal())
    {
        disconnectVk();
    }

    switch (errorResponse->code())
    {
    case ErrorResponse::CaptchaNeeded:
    {
        emit captcha(errorResponse->captchaSid(), errorResponse->captchaImg());
        break;
    }

    case ErrorResponse::HttpAuthorizationFailed:
    {
        setHttpsMode(true);

        if (!_queryList.isEmpty())
        {
            _isProcessing = false;
            execQuery();
        }
        else
        {
            getToken();
        }

        break;
    }

    case ErrorResponse::ValidationRequired:
    {
        clearPerishableQuery(ErrorResponse::ValidationRequired, "Validation required");

        switch (errorResponse->validationType())
        {
        case ErrorResponse::TwoFactorApp:
//        case ErrorResponse::TwoFactorSms: //todo request private method vk api
        {
            QString sid = errorResponse->validationSid();
            QString phone = errorResponse->validationPhone();
            bool isSms = errorResponse->validationType() == ErrorResponse::TwoFactorSms;
            QString uri = errorResponse->validationUri();

            if (isSms)
            {
                sendVerificationSms(sid);
            }

            emit verification(sid, phone, isSms, uri);
            break;
        }

        default:
        {
            emit validation(errorResponse->validationUri());
            break;
        }
        }

        break;
    }

    case ErrorResponse::TooManyRequestsPerSecond:
    {
        tooManyRequestsTimer->start();
        break;
    }

    default:
    {
        emit error(errorResponse->code(), errorResponse->msg(), errorResponse->global(), errorResponse->fatal());
        break;
    }
    }

    delete errorResponse;
}

void Connection::onError(const ErrorResponse::Error &code, const QString &msg)
{
    ErrorResponse *errorResponse = new ErrorResponse(code, msg);
    onError(errorResponse);
}
