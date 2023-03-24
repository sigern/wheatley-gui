QT += widgets serialport core gui quick quickwidgets

TARGET = terminal
TEMPLATE = app

SOURCES += \
    crc.cpp \
    main.cpp \
    mainwindow.cpp \
    SimpleXbox360Controller/simplexbox360controller.cpp \
    settingsdialog.cpp \
    console.cpp \
    gamepaddisplay.cpp

HEADERS += \
    crc.h \
    mainwindow.h \
    SimpleXbox360Controller/XInput.h \
    SimpleXbox360Controller/simplexbox360controller.h \
    settingsdialog.h \
    console.h \
    Queue.h \
    gamepaddisplay.h

FORMS += \
    mainwindow.ui \
    settingsdialog.ui

OTHER_FILES += \
    SimpleXbox360Controller/XInput.lib

RESOURCES += \
    terminal.qrc \
    resources.qrc

DISTFILES +=

unix:!macx|win32: LIBS += -L$$PWD/SimpleXbox360Controller/ -lXInput

INCLUDEPATH += $$PWD/SimpleXbox360Controller
DEPENDPATH += $$PWD/SimpleXbox360Controller
