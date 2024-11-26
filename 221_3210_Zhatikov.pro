QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# Если используется Qt 6 и выше, отключаем устаревшие API (если нужно)
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000


SOURCES += \
    addrecorddialog.cpp \
    main.cpp \
    mainwindow.cpp \
    pindialog.cpp

HEADERS += \
    addrecorddialog.h \
    mainwindow.h \
    pindialog.h

FORMS += \
    addrecorddialog.ui \
    mainwindow.ui \
    pindialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
