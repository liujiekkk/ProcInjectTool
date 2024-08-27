QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    basetable.cpp \
    dlg_dlllist.cpp \
    dlgwintool.cpp \
    dllmoduletable.cpp \
    injector.cpp \
    main.cpp \
    mainwindow.cpp \
    processmanager.cpp \
    processtable.cpp \
    versiondialog.cpp \
    windowselector.cpp

HEADERS += \
    basetable.h \
    common.h \
    dlg_dlllist.h \
    dlgwintool.h \
    dllmoduletable.h \
    injector.h \
    mainwindow.h \
    processmanager.h \
    processtable.h \
    versiondialog.h \
    windowselector.h

LIBS += \
    -luser32

FORMS += \
    dlg_dlllist.ui \
    dlgwintool.ui \
    mainwindow.ui \
    versiondialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    icons.qrc
