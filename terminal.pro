QT += widgets serialport core gui quick quickwidgets

TARGET = terminal
TEMPLATE = app

SOURCES += \
    frameparserthread.cpp \
    main.cpp \
    mainwindow.cpp \
    SimpleXbox360Controller/simplexbox360controller.cpp \
    senderthread.cpp \
    settingsdialog.cpp \
    console.cpp \
    serialhandler.cpp \
    gamepaddisplay.cpp

HEADERS += \
    frameparserthread.h \
    mainwindow.h \
    SimpleXbox360Controller/XInput.h \
    SimpleXbox360Controller/simplexbox360controller.h \
    senderthread.h \
    settingsdialog.h \
    console.h \
    serialhandler.h \
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
