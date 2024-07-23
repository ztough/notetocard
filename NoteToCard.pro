QT       += core gui webenginewidgets webchannel network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++20
CONFIG += qt_windows_winrt
QMAKE_CXXFLAGS += /await:strict
win32:LIBS += -lwindowsapp -lruntimeobject -lOLEAUT32
# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
RC_ICONS = logo.ico
SOURCES += \
    bridge.cpp \
    grammar.cpp \
    htmltemplate.cpp \
    main.cpp \
    mainwindow.cpp \
    note.cpp \
    notewebenginepage.cpp

HEADERS += \
    bridge.h \
    grammar.h \
    htmltemplate.h \
    mainwindow.h \
    note.h \
    notewebenginepage.h

FORMS += \
    grammar.ui \
    htmltemplate.ui \
    mainwindow.ui \
    note.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc
