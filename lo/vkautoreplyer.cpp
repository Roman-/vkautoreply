#include "vkautoreplyer.h"
#include "arma_logger.h"
#include "languageprocessing.h"
#include <QFile>

using namespace arma_logger;
using namespace vk_api;

VkAutoReplyer::VkAutoReplyer(const QString &token,
                             const QString &loLangPath,
                             int timerInterval):
    token_(token),
    loLangDbPath_(loLangPath)
{
    QFile f(loLangPath);
    if (!f.exists())
    {
        log("loLang file does not exist: " + loLangPath, lpError);
        exit(1);
    }

    timer_.setInterval(timerInterval);

    connect(&timer_, &QTimer::timeout, this, &VkAutoReplyer::update);

    srand(time(0));
}

void VkAutoReplyer::start()
{
    timer_.start();
}

void VkAutoReplyer::update()
{
    QList<VkMessage> messages = getUnreadMessages(token_);

    for (const VkMessage& m: messages) {
        QString reply = loLangGetReply(m.body, loLangDbPath_);
        if (reply != "")
        {
            // mark as read
            markAsRead(m.userId, m.id, token_);
            // reply
            sendMessage(reply, m.userId, token_);

            // get sender name for log
            VkUser sender = getUserById(m.userId, token_);
            log(sender.firstName + " " + sender.lastName + ": " +
                m.body + " --> " + reply, arma_logger::lpInfo);
        }
    }
}
