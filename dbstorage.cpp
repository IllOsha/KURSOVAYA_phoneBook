#include "dbstorage.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>

DBStorage::DBStorage(const QString &dbname, const QString &user, const QString &password, const QString &host, int port) {
    db = QSqlDatabase::addDatabase("QPSQL", QString("phonebook_conn_%1").arg(reinterpret_cast<uintptr_t>(this)));
    db.setDatabaseName(dbname);
    db.setUserName(user);
    db.setPassword(password);
    db.setHostName(host);
    db.setPort(port);
}

DBStorage::~DBStorage() {
    if (db.isOpen()) db.close();
    QSqlDatabase::removeDatabase(db.connectionName());
}

bool DBStorage::open() {
    if (!db.open()) {
        qWarning() << "DB open error:" << db.lastError().text();
        return false;
    }
    
    QSqlQuery q(db);
    QString create =
        "CREATE TABLE IF NOT EXISTS contacts ("
        "id SERIAL PRIMARY KEY,"
        "firstname TEXT,"
        "lastname TEXT,"
        "middlename TEXT,"
        "birthday TEXT,"
        "email TEXT,"
        "address TEXT"
        ");";
    if (!q.exec(create)) {
        qWarning() << "create table error:" << q.lastError().text();
        return false;
    }
    QString createPhones =
        "CREATE TABLE IF NOT EXISTS phones ("
        "id SERIAL PRIMARY KEY,"
        "contact_id INTEGER REFERENCES contacts(id) ON DELETE CASCADE,"
        "label TEXT,"
        "number TEXT"
        ");";
    if (!q.exec(createPhones)) {
        qWarning() << "create phones table error:" << q.lastError().text();
        return false;
    }
    return true;
}

void DBStorage::close() {
    if (db.isOpen()) db.close();
}

bool DBStorage::saveAll(const std::vector<contacts> &all) {
    if (!db.isOpen() && !open()) return false;

    QSqlQuery q(db);
    if (!q.exec("TRUNCATE TABLE phones CASCADE; TRUNCATE TABLE contacts RESTART IDENTITY CASCADE;")) {
        qWarning() << "truncate error:" << q.lastError().text();
        return false;
    }
    for (const auto &c : all) {
        QSqlQuery ins(db);
        ins.prepare("INSERT INTO contacts (firstname, lastname, middlename, birthday, email, address) VALUES (:fn, :ln, :mn, :bd, :em, :addr) RETURNING id;");
        ins.bindValue(":fn", QString::fromStdString(c.firstname));
        ins.bindValue(":ln", QString::fromStdString(c.lastname));
        ins.bindValue(":mn", QString::fromStdString(c.middlename));
        ins.bindValue(":bd", QString::fromStdString(c.birthday));
        ins.bindValue(":em", QString::fromStdString(c.email));
        ins.bindValue(":addr", QString::fromStdString(c.address));
        if (!ins.exec()) {
            qWarning() << "insert contact error:" << ins.lastError().text();
            return false;
        }
        if (!ins.next()) continue;
        int id = ins.value(0).toInt();

        for (const auto &ph : c.phones) {
            QSqlQuery ip(db);
            ip.prepare("INSERT INTO phones (contact_id, label, number) VALUES (:cid, :lab, :num);");
            ip.bindValue(":cid", id);
            ip.bindValue(":lab", QString::fromStdString(ph.label));
            ip.bindValue(":num", QString::fromStdString(ph.number));
            if (!ip.exec()) {
                qWarning() << "insert phone error:" << ip.lastError().text();
                // продолжаем, но отмечаем ошибку))
            }
        }
    }
    return true;
}

std::vector<contacts> DBStorage::loadAll() {
    std::vector<contacts> res;
    if (!db.isOpen() && !open()) return res;

    QSqlQuery q(db);
    if (!q.exec("SELECT id, firstname, lastname, middlename, birthday, email, address FROM contacts ORDER BY id;")) return res;

    while (q.next()) {
        contacts c;
        int id = q.value(0).toInt();
        c.firstname = q.value(1).toString().toStdString();
        c.lastname = q.value(2).toString().toStdString();
        c.middlename = q.value(3).toString().toStdString();
        c.birthday = q.value(4).toString().toStdString();
        c.email = q.value(5).toString().toStdString();
        c.address = q.value(6).toString().toStdString();
      
        QSqlQuery pq(db);
        pq.prepare("SELECT label, number FROM phones WHERE contact_id = :cid ORDER BY id;");
        pq.bindValue(":cid", id);
        if (pq.exec()) {
            while (pq.next()) {
                PhoneNumber ph;
                ph.label = pq.value(0).toString().toStdString();
                ph.number = pq.value(1).toString().toStdString();
                c.phones.push_back(ph);
            }
        }
        res.push_back(std::move(c));
    }
    return res;
}
