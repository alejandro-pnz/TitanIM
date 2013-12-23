#include "dialogitem.h"

DialogItemPrivate::DialogItemPrivate()
{
    _id = 0;
    _groupChatHandler = 0;
}

DialogItemPrivate::~DialogItemPrivate()
{
    if (_groupChatHandler)
    {
        delete _groupChatHandler;
    }
}

int DialogItemPrivate::id() const
{
    return _id;
}

QString DialogItemPrivate::displayName() const
{
    return isGroupChat() ? _groupChatHandler->title() : _profile->fullName();
}

QStringList DialogItemPrivate::decoration() const
{
    return isGroupChat() ? _groupChatHandler->avatars() : QStringList(_profile->photoMediumRect());
}

bool DialogItemPrivate::isGroupChat() const
{
    return _groupChatHandler;
}

ProfileItem DialogItemPrivate::profile() const
{
    return _profile;
}

void DialogItemPrivate::setProfile(const ProfileItem profile)
{
    if (_profile.data() != profile.data())
    {
        if (_profile)
        {
            disconnect(_profile.data(), SIGNAL(propertyChanged(int,QString)), this, SLOT(onProfilePropertyChanged(int,QString)));
        }

        _profile = profile;
        _id = _profile->uid();
        connect(profile.data(), SIGNAL(propertyChanged(int,QString)), this, SLOT(onProfilePropertyChanged(int,QString)));
        emit propertyChanged(_id, "profile");
    }
}

MessageItem DialogItemPrivate::message() const
{
    return _message;
}

void DialogItemPrivate::setMessage(const MessageItem message)
{
    if (_message.data() != message.data())
    {
        if (_message)
        {
            disconnect(_message.data(), SIGNAL(propertyChanged(int,QString)), this, SLOT(onMessagePropertyChanged(int,QString)));
        }

        _message = message;
        _id = _message->uid();
        connect(message.data(), SIGNAL(propertyChanged(int,QString)), this, SLOT(onMessagePropertyChanged(int,QString)));
        emit propertyChanged(_id, "message");
    }
}

GroupChatHandler *DialogItemPrivate::groupChatHandler() const
{
    return _groupChatHandler;
}

void DialogItemPrivate::setGroupChatHandler(GroupChatHandler *groupChatHandler)
{
    if (_groupChatHandler != groupChatHandler)
    {
        if (_groupChatHandler)
        {
            disconnect(_groupChatHandler, SIGNAL(propertyChanged(int,QString)), this, SLOT(onGroupChatPropertyChanged(int,QString)));
            delete _groupChatHandler;
        }

        _groupChatHandler = groupChatHandler;
        _id = groupChatHandler->id();
        connect(_groupChatHandler, SIGNAL(propertyChanged(int,QString)), this, SLOT(onGroupChatPropertyChanged(int,QString)));
        emit propertyChanged(_id, "groupChat");
    }
}

void DialogItemPrivate::onProfilePropertyChanged(const int uid, const QString &propertyName)
{
    QString property = QString("profile.%1").arg(propertyName);
    emit propertyChanged(_id, property);
}

void DialogItemPrivate::onMessagePropertyChanged(const int mid, const QString &propertyName)
{
    QString property = QString("message.%1").arg(propertyName);
    emit propertyChanged(_id, property);
}

void DialogItemPrivate::onGroupChatPropertyChanged(const int chatId, const QString &propertyName)
{
    QString property = QString("groupChat.%1").arg(propertyName);
    emit propertyChanged(_id, property);
}
