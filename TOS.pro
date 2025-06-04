QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Character.cpp \
    Enemy.cpp \
    FinishStageWidget.cpp \
    GameController.cpp \
    GameStageWidget.cpp \
    Gem.cpp \
    PauseWidget.cpp \
    PrepareStageWidget.cpp \
    main.cpp \
    MainWindow.cpp

HEADERS += \
    Character.h \
    Enemy.h \
    FinishStageWidget.h \
    GameController.h \
    GameStageWidget.h \
    Gem.h \
    PauseWidget.h \
    PrepareStageWidget.h \
    MainWindow.h

FORMS += \
    mainwindow.ui

TRANSLATIONS += \
    TOS_zh_TW.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    data.qrc
