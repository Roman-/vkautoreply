#include "vkapi.h"
#include "arma_logger.h"
#include "languageprocessing.h"
#include "waitforsignalhelper.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrlQuery>
#include <QFile>
#include <QUrl>

using namespace arma_logger;

namespace vk_api {

QString VkGlobals::defaultToken = "";

void VkGlobals::setDefaultToken(const QString &token)
{
    VkGlobals::defaultToken = token;
}

QString VkGlobals::getDefaultToken()
{
    return defaultToken;
}

QVariantMap callMethod(QString method, QVariantMap params, QString appToken)
{
    // composing URL
    QString url = "https://api.vk.com/method/" + method + "?";

    // adding GET parameters
    for (QVariantMap::const_iterator i = params.cbegin(); i != params.cend(); ++i)
    {
        url += i.key() + "=";
        QVariant v = i.value();
        if (v.type() == QVariant::Int)
            url += QString::number(v.toInt());
        else if (v.type() == QVariant::Double)
            url += QString::number(v.toDouble());
        else if (v.type() == QVariant::String)
            url += v.toString();
        url += "&";
    }

    // adding api version
    url += "v=5.37";

    // some methods dont require access token. Using empty strings for them
    if (appToken != "")
        url += "&access_token=" + appToken;

    log("call method url: " + url, lpTrace);

    QByteArray response = sendHttpRequest(url);

    if (response.size() == 0)
        return QVariantMap();

    const QVariantMap map = arma_logger::jsonStringToMap(response);

    if (map.contains("error"))
    {
        QString message = map["error"].toMap()["error_msg"].toString();
        if (message.size() != 0)
            log(message, lpError);
        else
            log("vk method returned error: " + response, lpError);
    }

    return map;
}

VkMessage::VkMessage(const QVariantMap &map):
    id(map["id"].toInt()),
    userId(map["user_id"].toInt()),
    dateTime(QDateTime::fromMSecsSinceEpoch(map["date"].toLongLong() * 1000L)),
    readState(map["read_state"].toInt()),
    out(map["out"].toInt()),
    title(map["title"].toString()),
    body(map["body"].toString()),
    attachments(map["attachments"]),
    fwd(map["fwd_messages"])
{
}

QString VkMessage::toString() const
{
    const int preLength = 15;
    QString s;
    s += dateTime.toString("d.MM hh:mm") + " ";
    s += out ? "->" : "<-";

    QString preview = s.size() > preLength ? body.left(preLength - 3) + "..." : body;
    s += preview;

    if (!attachments.isNull())
        s += "<attachment>";

    if (!readState)
        s += " [unread]";

    return s;
}

QList<VkMessage> getMessages(bool out, int offset, int count, QString appToken)
{
    QList<VkMessage> result;

    QVariantMap params = {
        {"out", int(out)},
        {"offset", offset},
        {"count", count},
    };

    QVariantMap reply = callMethod("messages.get", params, appToken);

    if (reply.contains("response"))
    {
        QVariantList qList = reply["response"].toMap()["items"].toList();

        if (qList.size() == 0)
            log("cant get messages(size=0): reply = " + mapToJsonString(reply), lpError);

        for (const QVariant& q: qList)
            result.push_back(VkMessage(q.toMap()));

        log("checked " + QString::number(result.size()) + " msg", lpTrace);
    }

    return result;
}

QList<VkMessage> getUnreadMessages(QString appToken)
{
    auto l = getMessages(false, 0, 100, appToken);
    l.erase(std::remove_if(l.begin(), l.end(), [](const VkMessage& m) {return m.readState;}), l.end());
    return l;
}

bool markAsRead(int personId, int messageId, QString appToken)
{
    QVariantMap response =
        callMethod("messages.markAsRead",
                   {
                       {"message_ids", messageId},
                       {"peer_id", personId}
                   },
                   appToken);

    log("marking as read result: " + mapToJsonString(response), arma_logger::lpDebug);

    return true;
}

int sendMessage(QString message, int personId, QString appToken)
{
    if (message.size() == 0)
        return false;

    message = QUrl::toPercentEncoding(message);

    QVariantMap params = {
        {"message", message},
        {"user_id", personId}
    };

    QVariantMap res = callMethod("messages.send", params, appToken);

    return res["response"].toInt();
}

bool likeProfilePicture(int userId, QString appToken)
{
    // 1. get person info
    QVariantMap json =
            callMethod("users.get",
               {
                   {"user_ids", userId},
                   {"fields", QString("photo_id")}
               }
               , appToken);

    QVariantList persons = json["response"].toList();

    if (persons.size() == 0)
    {
        log("cant get persons by id, response = " + mapToJsonString(json), lpError);
        return false;
    }

    // 2. get profile photo id

    QVariantMap person = persons[0].toMap();

    QString photoId = person["photo_id"].toString();
    if (photoId.indexOf("_") == -1)
    {
        log("cant get person`s photo", lpError);
        return true;
    }

    int itemId = photoId.split("_")[1].toInt();

    // 3. add like

    QVariantMap likesAddReply =
            callMethod("likes.add",
               {
                   {"owner_id", person["id"].toInt()},
                   {"item_id", itemId},
                   {"type", QString("photo")}
               }
               , appToken);

    int likes = likesAddReply["response"].toMap()["likes"].toInt();

    if (likes == 0)
        log("cant like photo: " + mapToJsonString(likesAddReply), lpError);

    return bool(likes);
}

VkUser::VkUser(const QVariantMap &map):
    id(map["id"].toInt()),
    firstName(map["first_name"].toString()),
    lastName(map["last_name"].toString())
{
}

VkUser::VkUser(int _id, const QString &_first, const QString &_last):
    id(_id),
    firstName(_first),
    lastName(_last)
{
}

VkUser VkUser::undefined()
{
    return VkUser(0,"undefined", "undefined");
}

VkUser getUserById(int id, QString appToken)
{
    QVariantMap response =
            callMethod("users.get",
               {
                   {"user_ids", id}
               }
               , appToken);

    QVariantList users = response["response"].toList();
    return (users.size() == 0) ? VkUser::undefined() : VkUser(users[0].toMap());

}

QByteArray sendHttpRequest(const QString &url, float timeoutSeconds) {
    QNetworkAccessManager mgr;

    QNetworkRequest req( QUrl( QString(url.toUtf8().data()) ) );
    QNetworkReply *reply = mgr.get(req);

    WaitForSignalHelper helper( mgr, SIGNAL(finished(QNetworkReply*)) );

    if ( !helper.wait( timeoutSeconds * 1000 ) )
    {
        log("http request timeout (" + QString::number(timeoutSeconds) + " sec)", lpError);
        return "";
    }


    if (reply->error() == QNetworkReply::NoError) {
        QByteArray result = reply->readAll();
        return result;
    }
    else {
        log("failed to send HTTP request: " + reply->errorString(), lpError);
        return "";
    }
}

} // namespace vk_api
