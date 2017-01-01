/** \file      arma_logger.h
 *  \author    Strakhov Roman <str.ra@ya.ru>
 *  \date      2016
 *  \brief     C++ logger
 */
#ifndef ARMA_LOGGER_H
#define ARMA_LOGGER_H

#include <QDebug>

namespace arma_logger
{

enum LoggingDetalization
{
    ldDefault = 1,   // specify priority only
    ldDetailed = 2,  // specify file name and line number when using qDebug()
    ldTime = 3,      // display priority and time (hh:mm.zzz)
};

enum LoggingPriority
{
    lpMinimal = 0,
    lpTrace = 10,
    lpDebug = 20,
    lpInfo = 30,
    lpWarn = 40,
    lpError = 50,
    lpFatal = 60
};

/// \brief The LoggingOptions class stores logging settings as static fields
class LoggingOptions
{
    LoggingOptions() = delete;
public:

    static LoggingDetalization detalization;

    static LoggingPriority minimalPriority;

    /// installs custom message handler for QDebug on initialization
    static bool init;

    /// if true, logger will never use color codes for priorities
    static bool ignoreColoring;
};

/**
 * \brief use setLoggingOptions to setup logger behavior
 * \param _detalization how detailed the output should be
 * \param _minimal_priority ignore messages with priority lower than minimal
 * \param _ignoreColoring do not use colour escape sequences for coloring priority strings in terminal
  */
bool setLoggingOptions(LoggingPriority _minimalPriority = lpInfo,
                       LoggingDetalization _detalization = ldTime,
                       bool _ignoreColoring = false);

/// \brief log outputs (logs) the string message
void log(const QString &msg, LoggingPriority priority = lpInfo);

/// \brief logs outputs map in JSON format
void log(const QMap<QString, QVariant>& map, LoggingPriority priority = lpDebug);

/// \brief mapToJsonString converts QMap to JSON string
QString mapToJsonString(const QVariantMap& map);

/// converts json string to QVariantMap
QVariantMap jsonStringToMap(const QByteArray& jsonString);

/// converts json string to QVariant
QVariant jsonStringToVariant(const QByteArray& jsonString);

/// \returns string with special characters added, which allow to display colored string in colnsole
QString wrapColor(const QString& s, Qt::GlobalColor color);

}


#endif // ARMA_LOGGER_H
