#include "mainwindow.h"
#include "validators.h"
#include <QMessageBox>
#include <QInputDialog>
#include <QDate>
#include <QFile>
#include <QTextStream>
#include <QHeaderView>

mainwindow::mainwindow(QWidget * parent) : QMainWindow(parent){ // настраиваем мэйн окно типо конструктор класса вызывает конструктор родительского класса
    setWindowTitle("phone alo");
    resize(800, 500);

    QWidget * central = new QWidget(this);// создаем рабочую область окна, ну и зис указывает на мэйн окно является родительским
    QBoxLayout * mainlayout = new QVBoxLayout(central);

    // ну тут вы чисто панельку верхнюю оформляем

    QHBoxLayout * toplayout = new QHBoxLayout;
    searchfield = new QLineEdit;
    searchfield -> setPlaceholderText("enter text ");
    findBtn = new QPushButton("search");
    addBtn = new QPushButton("add");
    delBtn = new QPushButton("delete");
    saveBtn = new QPushButton("save");
    toplayout -> addWidget(searchfield);
    toplayout -> addWidget(findBtn);
    toplayout -> addWidget(addBtn);
    toplayout -> addWidget(delBtn);
    toplayout -> addWidget(saveBtn);

    table = new QTableWidget; // ну тут новый объект таблицы в фрэймворке
    table -> setColumnCount(7);
    table->setHorizontalHeaderLabels({"LASTNAME", "FIRSTNAME", "MIDDLENAME", "BIRTHDAY", "EMAIL", "ADDRESS", "PHONES"});
    table->horizontalHeader()->setStretchLastSection(true);// ну типо получаем доступ к гориз заголовку таблицы, во второй чвсти типо растягиваем последнюю колону

    mainlayout -> addLayout(toplayout);
    mainlayout -> addWidget(table);
    setCentralWidget(central);

    QFile styleFile(":/style:qss");
    if (styleFile.open(QFile::ReadOnly))
        setStyleSheet(styleFile.readAll());

    connect(addBtn, &QPushButton :: clicked, this, &mainwindow::onadd);
    connect(delBtn, &QPushButton :: clicked, this, &mainwindow :: ondelete);
    connect(findBtn, &QPushButton :: clicked, this, &mainwindow::onfind);
    connect(saveBtn, &QPushButton :: clicked, this, &mainwindow :: onsave);

    pb.loadfromfile("phonebook.db");//бла бла загружаем данные из файла
    refreshtable();// обнова отобрадения таблицы
}

void mainwindow::refreshtable(){
    table -> setRowCount(static_cast <int> (pb.size()));// ну тут типо уст кол-во строк в таблице
    for (int i = 0; i < static_cast<int> (pb.size()); ++i){
        const auto &c = pb.at(i);// получает ссылку на итый)) контакт

        table -> setItem(i, 0, new QTableWidgetItem(QString::fromStdString(c.lastname))); //вот это 1,2,3,4,5,6 - типо коллонка какая
        table -> setItem(i, 1, new QTableWidgetItem(QString::fromStdString(c.firstname)));
        table -> setItem(i, 2, new QTableWidgetItem(QString::fromStdString(c.middlename)));
        table -> setItem(i, 3, new QTableWidgetItem(QString::fromStdString(c.birthday)));
        table -> setItem(i, 4, new QTableWidgetItem(QString::fromStdString(c.email)));
        table -> setItem(i, 5, new QTableWidgetItem(QString::fromStdString(c.address)));

        QString phonesStr;
               for (const auto& p : c.phones)
                   phonesStr += QString::fromStdString(p.label + ": " + p.number + " ");
               table->setItem(i, 6, new QTableWidgetItem(phonesStr.trimmed()));
    }
}

void mainwindow::onadd() {
    contacts c;
    bool ok = false;
    QString s;

    s = QInputDialog::getText(this, "LASTNAME", "enter last name:", QLineEdit::Normal, "", &ok);
    if (!ok || s.isEmpty()) return;
    c.lastname = s.toStdString();
    if (!Validators::validname(c.lastname)) {
        QMessageBox::warning(this, "error", "invalid last name format!");
        return;
    }

    s = QInputDialog::getText(this, "FIRSTNAME", "enter first name:", QLineEdit::Normal, "", &ok);
    if (!ok || s.isEmpty()) return;
    c.firstname = s.toStdString();
    if (!Validators::validname(c.firstname)) {
        QMessageBox::warning(this, "error", "invalid first name format!");
        return;
    }

    s = QInputDialog::getText(this, "MIDDLENAME", "enter middle name:", QLineEdit::Normal, "", &ok);
    if (!ok) return;
    c.middlename = s.toStdString();
    if (!c.middlename.empty() && !Validators::validname(c.middlename)) {
        QMessageBox::warning(this, "error", "invalid middle name format!");
        return;
    }

    s = QInputDialog::getText(this, "EMAIL", "enter email:", QLineEdit::Normal, "", &ok);
    if (!ok || s.isEmpty()) return;
    c.email = s.toStdString();
    if (!Validators::valiemail(c.email)) {
        QMessageBox::warning(this, "error", "invalid email format!");
        return;
    }

    s = QInputDialog::getText(this, "BIRTHDAY", "enter birthday (DD.MM.YYYY):", QLineEdit::Normal, "", &ok);
    if (!ok || s.isEmpty()) return;
    c.birthday = s.toStdString();

    QDate date = QDate::fromString(QString::fromStdString(c.birthday), "dd.MM.yyyy");
    if (!date.isValid() || date > QDate::currentDate()) {
        QMessageBox::warning(this, "error", "invalid or future birthday date!");
        return;
    }

    s = QInputDialog::getText(this, "ADDRESS", "enter address:", QLineEdit::Normal, "", &ok);
    if (!ok || s.isEmpty()) return;
    c.address = s.toStdString();

    PhoneNumber p;
    s = QInputDialog::getText(this, "PHONE LABEL", "enter phone label (home/work/etc):", QLineEdit::Normal, "", &ok);
    if (!ok) return;
    p.label = s.toStdString();

    s = QInputDialog::getText(this, "PHONE NUMBER", "enter phone number (+7... or 8...):", QLineEdit::Normal, "", &ok);
    if (!ok || s.isEmpty()) return;
    p.number = s.toStdString();

    if (!Validators::validname(c.firstname) ||
            !Validators::validname(c.lastname) ||
            !Validators::valiemail(c.email) ||
            !Validators::validbirthday(c.birthday))
        {
            QMessageBox::warning(this, "error","troubles((.\n" "lastname?, name?, email or BRTH.");
            return;
        }

    c.phones.push_back(p);

    pb.addcontact(std::move(c));
    refreshtable();
    QMessageBox::information(this, "UCPEH", "contact added successfully!");
}

void mainwindow::ondelete(){
    int row = table -> currentRow(); // получает номер выбранной строки
    if (row < 0){
        QMessageBox::warning(this, "error", "choose contact for delete");
        return;
    }
    pb.removebyindex(row);
    refreshtable();
}

void mainwindow:: onfind(){
    QString q = searchfield -> text();// ТУТ ТИПО текст из поля получаем
    if (q.isEmpty())
        return;
    auto res = pb.findbyname(q.toStdString()); // ищем в телефонной книжке

    for (int i = 0; i < table -> rowCount(); ++i) table -> setRowHidden(i, true); // скрываем табл
    for (auto idx : res) table -> setRowHidden ( static_cast <int>(idx), false); // тут только найденные

}

void mainwindow::onsave(){
    pb.savetofile("phonebook.db");
    QMessageBox :: information(this, "save", "I SAVE ALL YRA!");
}
