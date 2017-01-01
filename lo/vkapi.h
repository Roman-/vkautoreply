/** \file      vkapi.h
 *  \author    Strakhov Roman <str.ra@ya.ru>
 *  \date      2016
 *  \brief     A c++ wrapper for some Vk API functions and objects
 */
#ifndef VKAPI_H
#define VKAPI_H
#include <QTimer>
#include <QDateTime>
#include <QVariant>

namespace vk_api {

/**
 * @brief sendHttpRequest sends HTTP GET request
 * @return HTTP response as bytearray; returns empty bytearray if error occured
 */
QByteArray sendHttpRequest(const QString& url, float timeoutSeconds = 15);

struct VkMessage
{
    /// construct VkMessage from vk Private Message object: https://vk.com/dev/objects/message
    explicit VkMessage(const QVariantMap& map);

    /// \returns basic info
    QString toString() const;

    int id;
    int userId;
    QDateTime dateTime;
    bool readState;
    bool out;
    QString title;
    QString body;

    QVariant attachments;
    QVariant fwd;
};

struct VkUser
{
    /// construct VkUser from vk user object: https://vk.com/dev/objects/user
    explicit VkUser(const QVariantMap& map);

    VkUser(int _id, const QString& _first, const QString& _last);

    static VkUser undefined();

    int id;
    QString firstName;
    QString lastName;
};

class VkGlobals
{
    static QString defaultToken;
public:
    /// setups application token that would be used as default argument
    static void setDefaultToken(const QString& token);

    static QString getDefaultToken();
};


/**
 * @brief callMethod calls method specified in VK API documentation
 * example call: vk_api::callMethod("messages.get", {{"out", 0}, {"count", 10}})
 * @param method method name
 * @param params method parameters for http request
 * @param appToken application token
 * @returns map with fields specified in http reply json
 * @returns empty map if http request failed
 */
QVariantMap callMethod(QString method, QVariantMap params, QString appToken = VkGlobals::getDefaultToken());

/**
 * @brief getMessages calls messages.get vk method
 * @param out true, if need to obtain outcome messages
 * @param offset messages id offset
 * @param count amount of messages to get (max. 100)
 * @param appToken application token
 * @return list of VkMessage objects
 */
QList<VkMessage> getMessages(bool out = false, int offset = 0, int count = 20, QString appToken = VkGlobals::getDefaultToken());

/**
 * @brief getUnreadMessages checks last 100 incoming messages and searches for unread
 * @return list of VkMessage objects with readState==false
 */
QList<VkMessage> getUnreadMessages(QString appToken = VkGlobals::getDefaultToken());

/**
 * @brief markAsRead marks message as read
 * @param personId id of the person whose dialog contains the message
 * @param messageId id of the message
 * @return true if message was successfully marked as read
 */
bool markAsRead(int personId, int messageId, QString appToken = VkGlobals::getDefaultToken());

/**
 * @brief sendMessage sends text message to a user
 * @param message message text
 * @param personId recepient vk id
 * @return sent message id; 0 if message wasn't sent
 */
int sendMessage(QString message, int personId, QString appToken = VkGlobals::getDefaultToken());

/**
 * @brief likeProfilePicture adds "like" to person's profile photo
 * @param userId id of user whose photo we will like
 * @return true if like was added
 */
bool likeProfilePicture(int userId, QString appToken = VkGlobals::getDefaultToken());

VkUser getUserById(int id, QString appToken = VkGlobals::getDefaultToken());

} // namespace vk_api

#endif // VKAPI_H
