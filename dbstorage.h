#pragma once
#include <QString>
#include <vector>
#include "contacts.h"
#include <QSqlDatabase>

class DBStorage {
    QSqlDatabase db;
public:
    DBStorage(const QString &dbname,
              const QString &user,
              const QString &password,
              const QString &host = "localhost",
              int port = 5432);

    ~DBStorage();

    bool open();
    void close();

    // сохраняет все контакты (простая реализация, можно расширять, потом через sql все сделаю, пока в блокноте лежит все)
    bool saveAll(const std::vector<contacts> &all);

    // загружает все контакты из БД
    std::vector<contacts> loadAll();

    bool isOpen() const { return db.isOpen(); }
};
