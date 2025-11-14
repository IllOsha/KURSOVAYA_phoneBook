#include "mainwindow.h"
#include "validators.h"
#include <QMessageBox>
#include <QInputDialog>
#include <QDate>
#include <QFile>
#include <QTextStream>
#include <QHeaderView>
#include <QAbstractItemView>
#include <QDialogButtonBox>

mainwindow::mainwindow(QWidget * parent) : QMainWindow(parent){
    setWindowTitle("phone alo");
    resize(800, 500);

    QWidget * central = new QWidget(this);
    QBoxLayout * mainlayout = new QVBoxLayout(central);

    QHBoxLayout * toplayout = new QHBoxLayout;
    searchfield = new QLineEdit;
    searchfield->setPlaceholderText("enter text ");
    findBtn = new QPushButton("search");
    QPushButton * resetBtn = new QPushButton("reset"); 
    addBtn = new QPushButton("add");
    delBtn = new QPushButton("delete");
    saveBtn = new QPushButton("save");

    toplayout->addWidget(searchfield);
    toplayout->addWidget(findBtn);
    toplayout->addWidget(resetBtn);
    toplayout->addWidget(addBtn);
    toplayout->addWidget(delBtn);
    toplayout->addWidget(saveBtn);

    table = new QTableWidget;
    table->setColumnCount(7);
    table->setHorizontalHeaderLabels({"LASTNAME", "FIRSTNAME", "MIDDLENAME", "BIRTHDAY", "EMAIL", "ADDRESS", "PHONES"});
    table->horizontalHeader()->setStretchLastSection(true);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setSelectionMode(QAbstractItemView::SingleSelection);

    mainlayout->addLayout(toplayout);
    mainlayout->addWidget(table);
    setCentralWidget(central);

    QFile styleFile(":/style.qss");
    if (styleFile.open(QFile::ReadOnly))
        setStyleSheet(styleFile.readAll());

    connect(addBtn, &QPushButton::clicked, this, &mainwindow::onadd);
    connect(delBtn, &QPushButton::clicked, this, &mainwindow::ondelete);
    connect(findBtn, &QPushButton::clicked, this, &mainwindow::onfind);
    connect(saveBtn, &QPushButton::clicked, this, &mainwindow::onsave);
    connect(resetBtn, &QPushButton::clicked, [this](){ refreshtable(); searchfield->clear(); }); // 🔹 кнопка Reset

    pb.loadfromfile("phonebook.db");
    refreshtable();
}

void mainwindow::refreshtable(){
    table->setRowCount(static_cast<int>(pb.size()));
    for (int i = 0; i < static_cast<int>(pb.size()); ++i){
        const auto &c = pb.at(i);

        table->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(c.lastname)));
        table->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(c.firstname)));
        table->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(c.middlename)));
        table->setItem(i, 3, new QTableWidgetItem(QString::fromStdString(c.birthday)));
        table->setItem(i, 4, new QTableWidgetItem(QString::fromStdString(c.email)));
        table->setItem(i, 5, new QTableWidgetItem(QString::fromStdString(c.address)));

        QString phonesStr;
        for (const auto& p : c.phones)
            phonesStr += QString::fromStdString(p.label + ": " + p.number + " ");
        table->setItem(i, 6, new QTableWidgetItem(phonesStr.trimmed()));

        table->setRowHidden(i, false); // на случай, если поиск скрывал строки
    }
}

void showError(const QString &message) {
    QMessageBox msg;
    msg.setIcon(QMessageBox::Critical);
    msg.setWindowTitle("Error");
    msg.setText("<span style='color:black; font-size:13px;'>" + message + "</span>");
    msg.exec();
}

static QString getInputWithBlackPlaceholder(QWidget *parent, const QString &title, const QString &placeholder) {
    QDialog dialog(parent);
    dialog.setWindowTitle(title);
    dialog.setModal(true);

    QVBoxLayout layout(&dialog);
    QLabel label("Enter " + title.toLower() + ":");
    layout.addWidget(&label);

    // Поле ввода с белым текстом и чёрным placeholder
    QLineEdit lineEdit;
    lineEdit.setPlaceholderText(placeholder);
    lineEdit.setStyleSheet(
        "QLineEdit { color: white; }"              // текст, который вводит пользователь — белый
        "QLineEdit::placeholder { color: black; }" // подсказка (placeholder) — чёрная
    );
    layout.addWidget(&lineEdit);

    QDialogButtonBox buttons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    layout.addWidget(&buttons);

    QObject::connect(&buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    QObject::connect(&buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted)
        return lineEdit.text();
    return "";
}
QString mainwindow::getValidName(const QString &title, bool allowEmpty){
    while (true){
        QString s = getInputWithBlackPlaceholder(this, title, "Enter " + title.toLower());
        if (s.isEmpty()) return "";

        s = s.simplified();
        s = s.remove(QRegularExpression("\\d")); // убираем цифры

        QStringList parts = s.split(QRegularExpression("\\s+"), QString::SkipEmptyParts);
        QString result;
        for (const auto &p : parts)
            result += p;
        s = result;

        if (s.isEmpty() && allowEmpty) return "";
        if (!s.isEmpty() && Validators::validname(s.toStdString())) return s;

        showError("Invalid " + title.toLower() + "! Only letters and hyphens allowed, must start with a letter.");
    }
}

QString mainwindow::getValidEmail(const QString &title){
    while (true){
        QString s = getInputWithBlackPlaceholder(this, title, "Enter email");
        if (s.isEmpty()) return "";

        s = s.trimmed();
        s.replace(QRegularExpression("\\s*@\\s*"), "@");

        if (!s.isEmpty() && Validators::valiemail(s.toStdString())) return s;

        showError("Invalid email! Must be valid format (example@mail.ru).");
    }
}

QString mainwindow::getValidBirthday(const QString &title){
    while (true){
        QString s = getInputWithBlackPlaceholder(this, title, "Enter birthday (DD.MM.YYYY)");
        if (s.isEmpty()) return "";

        s = s.trimmed();
        if (!s.isEmpty() && Validators::validbirthday(s.toStdString())) return s;

        showError("Invalid birthday! Format DD.MM.YYYY and must be before today.");
    }
}

QString mainwindow::getValidAddress(const QString &title){
    while (true){
        QString s = getInputWithBlackPlaceholder(this, title, "Enter address");
        if (s.isEmpty()) return "";

        s = s.trimmed();
        if (!s.isEmpty() && Validators::validaddress(s.toStdString())) return s;

        showError("Address cannot be empty!");
    }
}

void mainwindow::onadd() {
    contacts c;

    c.lastname = getValidName("LASTNAME").toStdString();
    if (c.lastname.empty()) return;

    c.firstname = getValidName("FIRSTNAME").toStdString();
    if (c.firstname.empty()) return;

    c.middlename = getValidName("MIDDLENAME", true).toStdString();

    c.email = getValidEmail("EMAIL").toStdString();
    if (c.email.empty()) return;

    c.birthday = getValidBirthday("BIRTHDAY").toStdString();
    if (c.birthday.empty()) return;

    c.address = getValidAddress("ADDRESS").toStdString();
    if (c.address.empty()) return;

    // PHONE
    PhoneNumber p;
    bool okLabel = true;
    while (okLabel) {
        QString label = getInputWithBlackPlaceholder(this, "PHONE LABEL", "Enter phone label (home/work/etc)");
        if (label.isEmpty()) break;
        label = label.trimmed();

        while (true) {
            QString number = getInputWithBlackPlaceholder(this, "PHONE NUMBER", "Enter phone number (+7... or 8...)");
            if (number.isEmpty()) { okLabel = false; break; }
            number = QString::fromStdString(Validators::normalize_phone(number.toStdString()));
            if (!Validators::validphone(number.toStdString())) {
                showError("Invalid phone number! Must start with +7 or 8 followed by 10 digits.");
                continue;
            }

            p.label = label.toStdString();
            p.number = number.toStdString();
            break;
        }
        break;
    }

    if (!p.number.empty()) c.phones.push_back(p);
    pb.addcontact(std::move(c));
    refreshtable();
    QMessageBox::information(this, "Success", "Contact added successfully!");
}

void mainwindow::ondelete(){
    int row = table->currentRow();
    if (row < 0){
        showError("Please select a contact to delete.");
        return;
    }
    pb.removebyindex(row);
    refreshtable();
}

void mainwindow::onfind(){
    QString q = searchfield->text();
    if (q.isEmpty()) return;
    auto res = pb.findbyname(q.toStdString());

    for (int i = 0; i < table->rowCount(); ++i)
        table->setRowHidden(i, true);
    for (auto idx : res)
        table->setRowHidden(static_cast<int>(idx), false);
}

void mainwindow::onsave(){
    pb.savetofile("phonebook.db");
    QMessageBox::information(this, "Save", "All data saved to phonebook.db!");
}
