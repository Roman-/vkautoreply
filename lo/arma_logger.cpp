#include "arma_logger.h"
#include <QCoreApplication>
#include <QDateTime>
#include <QFile>
#include <QJsonObject>
#include <QJsonDocument>

#include <iostream>

namespace arma_logger {

LoggingPriority LoggingOptions::minimalPriority = lpMinimal;
LoggingDetalization LoggingOptions::detalization = ldDetailed;
bool LoggingOptions::init = setLoggingOptions();
bool LoggingOptions::ignoreColoring = false;


QString priorityToString(LoggingPriority p);

/// \brief fMessageOutput will be called on qDebug(), qFatal() etc.
void fMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QString details = "";
    if (LoggingOptions::detalization == ldDetailed)
    {
        QString fullFileName = QString(context.file);
        QString fileName = fullFileName.mid(fullFileName.lastIndexOf("/") + 1);
        details = "[" + fileName + ", #" +QString::number(context.line) + "] ";
    }

    switch (type) {
    case QtDebugMsg:
        return log(details + msg, lpDebug);
    case QtWarningMsg:
        return log(details + msg, lpWarn);
    case QtCriticalMsg:
        return log(details + msg, lpFatal);
    case QtFatalMsg:
        return log(details + msg, lpFatal);
    default:
        return log(details + msg, lpInfo);
    }
}

QString mapToJsonString(const QVariantMap& map)
{
    QJsonObject jsonObj = QJsonObject::fromVariantMap(map);
    QJsonDocument doc(jsonObj);
    return doc.toJson(QJsonDocument::Compact);
}

void log(const QVariant& variant, LoggingPriority priority)
{
    // если приоритет сообщения ниже минимального установленного, игнорируем вызов лог-функции
    if (priority < LoggingOptions::minimalPriority)
        return;

    if (LoggingOptions::detalization == ldTime)
        std::cout << QDateTime::currentDateTime().toString("hh:mm.zzz ").toStdString();

    std::string textPart = variant.canConvert<QString>() ? variant.toString().toStdString() : "(QVariant)";
    std::cout << priorityToString(priority).toStdString() << ": " << textPart << std::endl;
}

void log(const QString& msg, LoggingPriority priority)
{
    log(QVariant::fromValue(msg), priority);
}

void log(const QMap<QString, QVariant>& map, LoggingPriority priority)
{
    QString mapString = mapToJsonString(map);
    return log(mapString, priority);
}

QString priorityToString(LoggingPriority priority)
{
    switch(priority) {
    case lpTrace:
        return "TRACE";
    case lpDebug:
        return wrapColor("DEBUG", Qt::white);
    case lpInfo:
        return wrapColor("INFO", Qt::green);
    case lpWarn:
        return wrapColor("WARN", Qt::yellow);
    case lpError:
        return wrapColor("ERROR", Qt::red);
    case lpFatal:
        return wrapColor("FATAL", Qt::red);
    default:
        return wrapColor("LOG", Qt::white);
    }
}

bool setLoggingOptions(LoggingPriority _minimalPriority,
                       LoggingDetalization _detalization, bool _ignoreColoring)
{
    LoggingOptions::minimalPriority = _minimalPriority;
    LoggingOptions::detalization = _detalization;
    LoggingOptions::ignoreColoring = _ignoreColoring;

    // http://doc.qt.io/qt-5/qtglobal.html#qInstallMessageHandler
    qInstallMessageHandler(fMessageOutput);

    return true;
}

QVariant jsonStringToVariant(const QByteArray& jsonString)
{
    QJsonParseError e;
    QJsonDocument doc = QJsonDocument::fromJson(jsonString, &e);
    if (e.error != QJsonParseError::NoError)
        log("error parsing json: " + e.errorString(), lpError);
    return doc.toVariant();
}

QVariantMap jsonStringToMap(const QByteArray& jsonString)
{
    QVariant qvariant = jsonStringToVariant(jsonString);

    if (!qvariant.canConvert<QVariantMap>())
        log("jsonStringToMap: this is not a map: " + QString(jsonString), lpError);

    return qvariant.toMap();
}

QString wrapColor(const QString &s, Qt::GlobalColor color)
{
    if (LoggingOptions::ignoreColoring)
        return s;

    const QString escapeColor = "\033[0m";

    // character set for colouring string in terminal: http://tldp.org/HOWTO/Bash-Prompt-HOWTO/x329.html
    QString colorSeq;

    switch (color)
    {
    case Qt::black:
        colorSeq = "\033[1;30m";
        break;
    case Qt::red:
        colorSeq = "\033[1;31m";
        break;
    case Qt::green:
        colorSeq = "\033[1;32m";
        break;
    case Qt::yellow:
        colorSeq = "\033[1;33m";
        break;
    case Qt::blue:
        colorSeq = "\033[1;34m";
        break;
    case Qt::cyan:
        colorSeq = "\033[1;36m";
        break;
    case Qt::white:
        colorSeq = "\033[1;37m";
        break;
    default:
        break;
    }

    return colorSeq + s + escapeColor;
}

} // namespace arma_logger
