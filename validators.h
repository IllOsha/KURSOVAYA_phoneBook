#pragma once
#include <string>
#include <regex>
#include <QDate>
#include <algorithm>
#include <cctype>
#include <QString>
#include <QChar>

namespace Validators {

inline std::string trim(const std::string& s) {
    size_t b = 0;
    while (b < s.size() && std::isspace(static_cast<unsigned char>(s[b]))) ++b;
    size_t e = s.size();
    while (e > b && std::isspace(static_cast<unsigned char>(s[e - 1]))) --e;
    return s.substr(b, e - b);
}
inline bool validname(const std::string& s) {
    QString t = QString::fromStdString(s).trimmed();
    if (t.isEmpty())
        return false;

    if (t.startsWith('-') || t.endsWith('-'))
        return false;

    bool firstIsLetter = false;
    for (int i = 0; i < t.size(); ++i) {
        QChar ch = t[i];

        if (ch.isLetter()) {
            if (i == 0)
                firstIsLetter = true;
            continue;
        }
        if (ch == '-' || ch.isSpace()) {
            continue;
        }

        return false;
    }
    return firstIsLetter;
}

inline bool validphone(const std::string& s) {
    auto t = trim(s);
    if (t.empty()) return false;

    static const std::regex re(
        R"(^(\+7|8)[ ]?(\(\d{3}\)|\d{3})[ ]?\d{3}[- ]?\d{2}[- ]?\d{2}$)"
    );
    return std::regex_match(t, re);
}

inline bool valiemail(const std::string& s) {
    auto t = trim(s);
    if (t.empty()) return false;

    static const std::regex re(
        R"(^[A-Za-z0-9]+([._%+-][A-Za-z0-9]+)*@[A-Za-z0-9-]+(\.[A-Za-z0-9-]+)*\.[A-Za-z]{2,}$)"
    );
    return std::regex_match(t, re);
}

inline bool validbirthday(const std::string& s) {
    auto t = trim(s);
    if (t.empty()) return false;

    QDate date = QDate::fromString(QString::fromStdString(t), "dd.MM.yyyy");
    if (!date.isValid()) return false;
    if (date >= QDate::currentDate()) return false;
    return true;
}

inline bool validaddress(const std::string& s) {
    auto t = trim(s);
    return !t.empty();
}
} 
