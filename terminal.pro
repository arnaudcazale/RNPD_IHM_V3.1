QT += \
    widgets \
    serialport \
    printsupport

CONFIG += qwt

requires(qtConfig(combobox))

TARGET = terminal
TEMPLATE = app

SOURCES += \
    displaywindow.cpp \
    main.cpp \
    mainwindow.cpp \
    plot.cpp \
    popupwindow.cpp \
    settingsdialog.cpp \
    console.cpp

HEADERS += \
    displaywindow.h \
    mainwindow.h \
    plot.h \
    popupwindow.h \
    settingsdialog.h \
    console.h

FORMS += \
    mainwindow.ui \
    settingsdialog.ui

RESOURCES += \
    terminal.qrc

target.path = $$[QT_INSTALL_EXAMPLES]/serialport/terminal
INSTALLS += target

DISTFILES +=
