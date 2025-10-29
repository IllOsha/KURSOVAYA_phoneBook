#pragma once
#include <QMainWindow> // я плакать буду, короче это для интерфейса
#include <QTableWidget>// таблица для отображения данных
#include <QPushButton>// ну тут кнопочки добавить
#include <QLineEdit>// поле для ввода текста
#include <QVBoxLayout>// крч для расположения элементов этот вертикально
#include <QHBoxLayout>// этот горизонтально
#include <QLabel>// текстовая метка тип <3

#include "phonebook.h"

class mainwindow : public QMainWindow{
    Q_OBJECT;// ало макрос для включения метаобъектов, типо как сигнал для динамических свойств и бла бла чтобы класс наследовался от QOBJECT
public:
    explicit mainwindow(QWidget *parent = nullptr); // explicit для неявных преобразований нужен, но вообще можно mainwindow w = nullptr, но как будто опасно будет, если будет не лень попробую
// * parent - короче родитльский виджет, который будет управлять памятью дочернного, ну короче база, чтобы автоматом управлять памятью
private slots:// ну слоты типо для обработки пользовательских действий
    void onadd();// ну тут понятно, кнопки добавляю
    void ondelete();
    void onfind();
    void onsave();
private:
    phonebook pb;

    QTableWidget * table;// таблица для контактов ну и дальше понятно
    QLineEdit * searchfield;
    QPushButton * addBtn;
    QPushButton * delBtn;
    QPushButton * saveBtn;
    QPushButton * findBtn;

    void refreshtable(); // для обновления таблицы
};

