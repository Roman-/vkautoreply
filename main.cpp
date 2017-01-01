#include <QCoreApplication>
#include <lo/arma_logger.h>
#include <lo/languageprocessing.h>
#include <lo/vkautoreplyer.h>
#include <QCommandLineOption>
#include <QCommandLineParser>

using namespace arma_logger;

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    app.setApplicationVersion("1.0");

    QCommandLineParser parser;
    parser.setApplicationDescription("Vk AutoReply bot");
    parser.addHelpOption();
    parser.addVersionOption();

    parser.addOptions({
        // An option with a value
        {{"t", "token"},
            QCoreApplication::translate("main", "Standalone application token"),
            QCoreApplication::translate("main", "token")},
        // reply rules (LoLanguage) file
        {{"p", "patterns"},
            QCoreApplication::translate("main", "Path to loLanguage patterns file"),
            QCoreApplication::translate("main", "patterns")},
        // delay
        {{"d", "delay"},
            QCoreApplication::translate("main", "Delay (in milliseconds) between requests"),
            QCoreApplication::translate("main", "delay")},
    });

    // Process the actual command line arguments given by the user
    parser.process(app);

    if (!parser.isSet("t")) {
        log("Token is not set. Use \"" + app.applicationName() + " -t <token>\"", lpError);
        exit(1);
    }
    QString token = parser.value("t");

    if (!parser.isSet("p")) {
        log("Patterns file is not set. Use \""
            + app.applicationName() + " -p path/to/patterns.txt\"", lpError);
        exit(1);
    }
    QString patternsPath = parser.value("p");

    int delay = parser.isSet("d") ? parser.value("d").toInt() : 1500;
    delay = std::max(delay, 1000);

    log("Starting VkAutoReplyer...", lpInfo);
    log("token = " + token.left(3) + "..." + token.right(3), lpInfo);
    log("path to reply patterns = " + patternsPath, lpInfo);
    log("delay = " + QString::number(delay) + " ms.", lpInfo);

    VkAutoReplyer bot(token, patternsPath, delay);
    bot.start();

    log("*** VkAutoReplyer running ***", lpInfo);

    return app.exec();
}
