QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    importcontrolparameters.cpp \
    main.cpp \
    mainwindow.cpp \
    testitemselection.cpp

HEADERS += \
    importcontrolparameters.h \
    mainwindow.h \
    testitemselection.h

FORMS += \
    importcontrolparameters.ui \
    mainwindow.ui \
    testitemselection.ui


win32:LIBS += -L"C:/Program Files (x86)/Windows Kits/10/Lib/10.0.19041.0/um/x86" -lUser32

win32:LIBS += -L"C:/Program Files (x86)/Windows Kits/10/Lib/10.0.19041.0/um/x86" -lodbc32

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../library_test_configuration/release/ -llibrary_test_configuration
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../library_test_configuration/debug/ -llibrary_test_configuration

INCLUDEPATH += $$PWD/../library_test_configuration/library_test_configuration
DEPENDPATH += $$PWD/../library_test_configuration/library_test_configuration

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../library_test_configuration/release/liblibrary_test_configuration.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../library_test_configuration/debug/liblibrary_test_configuration.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../library_test_configuration/release/library_test_configuration.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../library_test_configuration/debug/library_test_configuration.lib

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../library_test_configuration/packages/sqlite3_c_plus_plus.1.0.3/build/native/lib/Win32/v140/release/ -lsqlite3
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../library_test_configuration/packages/sqlite3_c_plus_plus.1.0.3/build/native/lib/Win32/v140/debug/ -lsqlite3

INCLUDEPATH += $$PWD/../library_test_configuration/packages/sqlite3_c_plus_plus.1.0.3/build/native/include
DEPENDPATH += $$PWD/../library_test_configuration/packages/sqlite3_c_plus_plus.1.0.3/build/native/include

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../library_test_configuration/packages/sqlite3_c_plus_plus.1.0.3/build/native/lib/Win32/v140/release/libsqlite3.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../library_test_configuration/packages/sqlite3_c_plus_plus.1.0.3/build/native/lib/Win32/v140/debug/libsqlite3.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../library_test_configuration/packages/sqlite3_c_plus_plus.1.0.3/build/native/lib/Win32/v140/release/sqlite3.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../library_test_configuration/packages/sqlite3_c_plus_plus.1.0.3/build/native/lib/Win32/v140/debug/sqlite3.lib

