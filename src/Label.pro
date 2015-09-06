#-------------------------------------------------
#
# Project created by QtCreator 2015-04-02T15:13:39
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Label
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    setparameterdialog.cpp

HEADERS  += mainwindow.h \
    setparameterdialog.h

FORMS    += mainwindow.ui \
    setparameterdialog.ui

INCLUDEPATH +=  D:/Libraries/opencv249/build/include/ \
                D:/Libraries/opencv249/build/include/opencv

LIBS += -LD:/Libraries/opencv249/build/x64/vc12/lib \
        -LD:/Libraries/opencv249/build/x64/vc12/bin \
        -lopencv_calib3d249 \
        -lopencv_contrib249 \
        -lopencv_core249 \
        -lopencv_features2d249 \
        -lopencv_flann249 \
        -lopencv_gpu249 \
        -lopencv_highgui249 \
        -lopencv_imgproc249 \
        -lopencv_legacy249 \
        -lopencv_ml249 \
        -lopencv_nonfree249 \
        -lopencv_objdetect249 \
        -lopencv_ocl249 \
        -lopencv_photo249 \
        -lopencv_stitching249 \
        -lopencv_superres249 \
        -lopencv_ts249 \
        -lopencv_video249 \
        -lopencv_videostab249 \
