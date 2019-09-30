#-------------------------------------------------
#
# Project created by QtCreator 2018-07-08T20:02:12
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = VideoProcess
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
INCLUDEPATH += E:/opencv/build/include

CONFIG(debug, debug|release): {
LIBS += -LE:/opencv/build/x64/vc12/lib \
-lopencv_core2411d \
-lopencv_imgproc2411d \
-lopencv_highgui2411d \
-lopencv_ml2411d \
-lopencv_video2411d \
-lopencv_features2d2411d \
-lopencv_calib3d2411d \
-lopencv_objdetect2411d \
-lopencv_contrib2411d \
-lopencv_legacy2411d \
-lopencv_flann2411d
} else:CONFIG(release, debug|release): {
LIBS += -LE:/opencv/build/x64/vc12/lib \
-lopencv_core2411 \
-lopencv_imgproc2411 \
-lopencv_highgui2411 \
-lopencv_ml2411 \
-lopencv_video2411 \
-lopencv_features2d2411 \
-lopencv_calib3d2411 \
-lopencv_objdetect2411 \
-lopencv_contrib2411 \
-lopencv_legacy2411 \
-lopencv_flann2411
}

SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui
