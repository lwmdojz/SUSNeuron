QT      += core gui
QT      += charts
QT      += bluetooth

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
QT      += network printsupport
CONFIG  += c++17

# large memory set
QMAKE_LFLAGS_WINDOWS += -Wl,--stack,32000000

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ble_finder.cpp \
    debugwindow.cpp \
    main.cpp \
    mainwindow.cpp \
    plot.cpp

HEADERS += \
    ble_finder.h \
    debugwindow.h \
    mainwindow.h \
    plot.h \

FORMS += \
    ble_finder.ui \
    debugwindow.ui \
    mainwindow.ui

TRANSLATIONS += \
    SUSNeuron_zh_CN.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
