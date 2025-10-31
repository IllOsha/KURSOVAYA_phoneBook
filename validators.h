#pragma once
#include <string>
#include <regex>
#include <QDate>
#include <algorithm>
#include <cctype>
#include <QString>
#include <QChar>
#include <QRegularExpression>
#include <set>

namespace Validators {

// trim: удалить ведущие/замыкающие пробелы
inline std::string trim(const std::string& s) {
    size_t b = 0;
    while (b < s.size() && std::isspace(static_cast<unsigned char>(s[b]))) ++b;
    size_t e = s.size();
    while (e > b && std::isspace(static_cast<unsigned char>(s[e - 1]))) --e;
    return s.substr(b, e - b);
}

inline std::string normalize_name(const std::string& s) {
    QString qs = QString::fromStdString(s).trimmed();

    qs.replace(QRegularExpression("\\s*-\\s*"), "-");// убрать пробелы вокруг дефисов

    qs.replace(QRegularExpression("\\s+"), " ");

    QStringList parts = qs.split(' ', QString::SkipEmptyParts); 
    QStringList outParts;
    for (int i = 0; i < parts.size();) {
        if (parts[i].size() == 1) {
            int j = i;
            QString merged;
            while (j < parts.size() && parts[j].size() == 1) {
                merged += parts[j];
                ++j;
            }
            if (merged.size() > 1) {
                outParts << merged;
            } else {
                outParts << parts[i];
            }
            i = j;
        } else {
            outParts << parts[i];
            ++i;
        }
    }
    QString result = outParts.join(' ');
    return result.toStdString();
}


inline std::string normalize_phone(const std::string& s) { // norm телефона удаляет пробелы, скобки, дефисы и табы
    std::string t;
    for (char ch : s) {
        if (ch == ' ' || ch == '(' || ch == ')' || ch == '-' || ch == '\t') continue;
        t.push_back(ch);
    }
    return t;
}

inline bool validname(const std::string& s) {
    std::string norm = normalize_name(s);
    if (norm.empty()) return false;

    QString t = QString::fromStdString(norm);

    if (t.startsWith('-') || t.endsWith('-')) return false;

    if (!t[0].isLetter()) return false;

    for (int i = 0; i < t.size(); ++i) {
        QChar ch = t[i];
        if (ch.isLetter() || ch.isDigit() || ch == '-' || ch.isSpace()) {
            continue;
        }
        return false;
    }
    return true;
}
inline bool validphone(const std::string& s) {
    std::string t = normalize_phone(s);
    static const std::regex re(R"(^(?:\+7|8)\d{10}$)");
    return std::regex_match(t, re);
}

inline std::string canonical_phone(const std::string& s) {
    return normalize_phone(s);
}

inline bool valiemail(const std::string& s) {
    std::string t = trim(s);
    if (t.empty()) return false;

    static const std::regex re(
        R"(^[A-Za-z0-9._%+\-]+@[A-Za-z0-9.-]+\.(ru|com|org|net|edu|gov|info|mail|gmail|yandex|bk|list|inbox)$)"
    );
    return std::regex_match(t, re);
}

inline bool isPopularEmailDomain(const std::string& s) {
    auto at = s.find('@');
    if (at == std::string::npos) return false;
    std::string domain = s.substr(at + 1);
    std::transform(domain.begin(), domain.end(), domain.begin(), [](unsigned char c){ return std::tolower(c); });

    static const std::set<std::string> popular = {
        "gmail.com","yandex.ru","mail.ru","hotmail.com","outlook.com","icloud.com",
        "yandex.com","gmail.ru","inbox.ru","list.ru","bk.ru","rambler.ru","ya.ru"
    };
    return popular.find(domain) != popular.end();
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
    return !trim(s).empty();
}

} 
