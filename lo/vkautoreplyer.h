#ifndef VKAUTOREPLYER_H
#define VKAUTOREPLYER_H

#include <QObject>
#include <QTimer>
#include "vkapi.h"

class VkAutoReplyer: public QObject {
    Q_OBJECT

public:
    /**
     * @brief VkAutoReplyer
     * @param token
     * @param loLangPath
     * @param timerInterval interval in milliseconds between checking for unread messages
     */
    VkAutoReplyer( const QString& token = vk_api::VkGlobals::getDefaultToken(),
                   const QString& loLangPath = "/tmp/lolang.txt",
                   int timerInterval = 1000
                  );

    // starting autorepli
    void start();

private:
    // timer for updates
    QTimer timer_;

    // application token
    QString token_;

    // path to file with loLang patterns
    QString loLangDbPath_;

private slots:

    // reads messages, marks as read and replies
    void update();

};
#endif // VKAUTOREPLYER_H
