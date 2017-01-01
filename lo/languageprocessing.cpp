#include "languageprocessing.h"
#include <lo/arma_logger.h>
#include <QFile>

int findOpenBracket(const QString& pattern, int closeBracketIndex,
                    QChar openBracketChar, QChar closeBracketChar)
{
    int level = 1, j = closeBracketIndex - 1;
    while (level != 0 && j > 0)
    {
        --j;
        if (pattern[j] == openBracketChar)
            --level;
        else if (pattern[j] == closeBracketChar)
            ++level;
    }
    return j;
}

QString simplifyInBrackets(const QString& s, QChar splitChar)
{
    QStringList list = s.split(splitChar);
    int index = rand() % list.size();
    return list[index];
}

QString loLangGenerate(QString pattern)
{
    const QString chars = QString("{}|?");
    pattern.replace(QString("??"), QString("⍰"));

    // 1. eliminate brackets starting from the most nested
    while (pattern.indexOf(chars[1]) != -1)
    {
        int c = pattern.indexOf(chars[1]); // )
        int o = findOpenBracket(pattern, c, chars[0], chars[1]);

        if (c == o)
            return "c=o";

        QString simplified = simplifyInBrackets(pattern.mid(o+1, c-o-1), chars[2]);

        if (c < pattern.size() - 1 && pattern[c+1] == chars[3])
        {
            // remove questionmark
            pattern.remove(c+1, 1);
            if (rand() % 2)
                simplified = "";
        }
        pattern.replace(o, c-o+1, simplified);
    }

    // 1.5 simplifying outer colon
    pattern = simplifyInBrackets(pattern, chars[2]);

    // 2. eliminate single character questionmarks
    while (pattern.indexOf(chars[3]) != -1)
    {
        int i = pattern.indexOf(chars[3]);

        // removing questionmark
        pattern.remove(i, 1);

        // removing char
        if (i != 0 && qrand() % 2)
            pattern.remove(i-1, 1); // remove char
    }
    return pattern.replace("⍰", "?");
}

QString loLangGetReply(const QString &input, const QString &repliesfilePath)
{
    const QString splitter = "%";
    const QString phraseLower = input.toLower();

    QFile file(repliesfilePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        log("cann\'t open patterns file " + repliesfilePath, arma_logger::lpError);
        return "";
    }

    while (!file.atEnd()) {
        QString line = file.readLine();
        QStringList parts = line.split(splitter);
        if (parts.size() != 2)
        {
            log("cant split line: " + line, arma_logger::lpWarn);
            return "";
        }
        QRegExp regex(parts[0]);
        QString reply = parts[1];
        if (reply[reply.size() - 1] == '\n')
            reply = reply.mid(0, reply.size() - 1);

        if (regex.indexIn(phraseLower) != -1)
            return loLangGenerate(reply);
    }

    // no regex satisfies input phrase
    return "";
}
