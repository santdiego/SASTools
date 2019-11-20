#-------------------------------------------------
#
# Project created by QtCreator 2016-08-31T18:23:13
#
#-------------------------------------------------

QT       += core gui
QT       += multimedia
QT       += multimediawidgets
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = VideoTransmissionV100
TEMPLATE = app
CONFIG += static
CONFIG += staticlib

SOURCES += main.cpp\
        mainwindow.cpp \
    hardwaredevice.cpp

HEADERS  += mainwindow.h \
    hardwaredevice.h

FORMS    += mainwindow.ui


win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../../../opencv/build/x64/vc15/lib/ -lopencv_world410
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../../../opencv/build/x64/vc15/lib/ -lopencv_world410d

INCLUDEPATH += $$PWD/../../../../../../../opencv/build/include
DEPENDPATH += $$PWD/../../../../../../../opencv/build/include

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../../../../../../opencv/build/x64/vc15/lib/libopencv_world410.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../../../../../../opencv/build/x64/vc15/lib/libopencv_world410d.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../../../../../../opencv/build/x64/vc15/lib/opencv_world410.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../../../../../../opencv/build/x64/vc15/lib/opencv_world410d.lib
