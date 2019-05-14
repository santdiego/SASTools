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

##DISTFILES +=

win32 {
    message("Using win32 configuration")
INCLUDEPATH += C:\opencvMingw\install\include

LIBS += -LC:\opencvMingw\install\x86\mingw\bin \
#    -lopencv_calib3d320\
    -lopencv_core320\
#    -lopencv_features2d320\
#    -lopencv_flann320\
    -lopencv_highgui320\
    -lopencv_imgcodecs320\
    -lopencv_imgproc320\
#    -lopencv_ml320\
#    -lopencv_objdetect320\
#     -lopencv_photo320\
#    -lopencv_shape320\
#    -lopencv_stitching320\
#    -lopencv_superres\
#    -lopencv_ts320\
#      -lopencv_video320\
#     -lopencv_videostab\
     -lopencv_videoio320
}

unix {
    message("Using unix configuration")

    INCLUDEPATH += /usr/local/include/opencv
    LIBS += -L/usr/local/lib \
    -lopencv_calib3d\
    -lopencv_core\
    -lopencv_features2d\
    -lopencv_flann\
    -lopencv_highgui\
    -lopencv_imgcodecs\
    -lopencv_imgproc\
    -lopencv_ml\
    -lopencv_objdetect\
    -lopencv_photo\
    -lopencv_shape\
    -lopencv_stitching\
    -lopencv_superres\
    -lopencv_ts\
    -lopencv_video\
    -lopencv_videoio\
    -lopencv_videostab
}
