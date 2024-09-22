QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    commonsettingsrandom.cpp \
    delegate_random_prof.cpp \
    main.cpp \
    mainwindow.cpp \
    profiletoinquiery.cpp \
    ramp.cpp

HEADERS += \
    commonsettingsrandom.h \
    delegate_random_prof.h \
    mainwindow.h \
    profiletoinquiery.h \
    ramp.h

FORMS += \
    commonsettingsrandom.ui \
    mainwindow.ui \
    profiletoinquiery.ui \
    ramp.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
