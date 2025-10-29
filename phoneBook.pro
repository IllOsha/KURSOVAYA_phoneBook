#  ПРОЕКТ: Телефонный справочник 
QT += core gui widgets

CONFIG += c++17
CONFIG -= app_bundle

# макрос для Qt 
DEFINES += USE_QT_GUI

TEMPLATE = app
TARGET = phoneBook

# тут исходники .cpp
SOURCES += \
    main.cpp \
    mainwindow.cpp \
    contacts.cpp \
    phonebook.cpp \
    filestorage.cpp


# .h - файлы все
HEADERS += \
    mainwindow.h \
    contacts.h \
    filestorage.h \
    icontactstorage.h \
    phonebook.h \
    validators.h


# ресурсы (.qrc)
RESOURCES += \
    resources.qrc

#  пути для include и общие флаги компиляции
INCLUDEPATH += $$PWD
QMAKE_CXXFLAGS += -Wall -Wextra -O2
