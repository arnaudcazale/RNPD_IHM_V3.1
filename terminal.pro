QT += \
    widgets \
    serialport \
    printsupport \
    multimedia \
    multimediawidgets

CONFIG += qwt

requires(qtConfig(combobox))

TARGET = terminal
TEMPLATE = app

SOURCES += \
    QRoundProgressBar.cpp \
    displaywindow.cpp \
    main.cpp \
    mainwindow.cpp \
    plot.cpp \
    popupwindow.cpp \
    resultwindow.cpp \
    sequencer.cpp \
    settingsdialog.cpp \
    console.cpp

HEADERS += \
    QRoundProgressBar.h \
    displaywindow.h \
    mainwindow.h \
    plot.h \
    popupwindow.h \
    resultwindow.h \
    sequencer.h \
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
