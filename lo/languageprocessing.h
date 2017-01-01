#ifndef LANGUAGEPROCESSING_H
#define LANGUAGEPROCESSING_H

#include <QString>

/// \brief generates phrase using loLanguale
/// loLang rules:
/// * {abc|def} will be either abc or def
/// * a?{bc}? will produce one of four phrases: "", "a", "bc", "abc"
/// * to use questionmark as a part of a phrase, duplicate it: "{A|a}re you there??" -> "Are you there?" or "are you there?"
QString loLangGenerate(QString pattern);

/// \param repliesFilePath path to file with regex->lolang rules.
/// \param input phrase to be replied
/// \returns empty string if file not found or no regex satisfies the input phrase
QString loLangGetReply(const QString& input, const QString& repliesfilePath);

#endif // LANGUAGEPROCESSING_H
