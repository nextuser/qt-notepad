QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets core5compat

CONFIG += c++17
ICON = app.icns

RC_ICONS = app.ico
RC_FILE += app.rc
# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    codeeditor.cpp \
    fileutil.cpp \
    highlighter.cpp \
    linenumberarea.cpp \
    main.cpp \
    mainwindow.cpp \
    searchdialog.cpp \
    tformdoc.cpp \
    tsubwindow.cpp

HEADERS += \
    ShowResult.h \
    codeeditor.h \
    fileinterface.h \
    fileutil.h \
    highlighter.h \
    linenumberarea.h \
    mainwindow.h \
    searchdialog.h \
    searchoptions.h \
    tformdoc.h \
    tsubwindow.h

FORMS += \
    mainwindow.ui \
    searchdialog.ui \
    tformdoc.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc

DISTFILES += \
    images/icons8-save-as-24.png
