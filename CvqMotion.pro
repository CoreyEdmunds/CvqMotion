#-------------------------------------------------
#
# Project created by QtCreator 2018-11-25T13:41:49
#
#-------------------------------------------------

lessThan(QT_MAJOR_VERSION, 5) {
    error("Use at least Qt version 5 (try \"qmake -qt5 ..\")")
}

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CvqMotion
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11
QMAKE_CXXFLAGS += -std=c++11

INCLUDEPATH += src/
OBJECTS_DIR=obj/
MOC_DIR=moc/
UI_DIR=ui/

SOURCES += \
        src/main.cpp \
        src/mainwindow.cpp \
    src/videoprocessor.cpp \
    src/entity.cpp \
    src/videoprocessorcontroller.cpp \
    src/detectionzone.cpp \
    src/cameraview.cpp \
    src/measurementdialog.cpp \
    src/detectionzonedialog.cpp \
    src/detectionslisttablemodel.cpp \
    src/aboutdialog.cpp \
    src/detectionsettingsdialog.cpp \
    src/devicecontrolwidget.cpp

HEADERS += \
        src/mainwindow.h \
    src/videoprocessor.h \
    src/entity.h \
    src/videoprocessorcontroller.h \
    src/detectionzone.h \
    src/cameraview.h \
    src/measurementdialog.h \
    src/detectionzonedialog.h \
    src/detectionslisttablemodel.h \
    src/aboutdialog.h \
    src/detectionsettingsdialog.h \
    src/videoprocessordetectionsettings.h \
    src/videoprocessorconstants.h \
    src/devicecontrolwidget.h

FORMS += \
    ui/mainwindow.ui \
    ui/measurementdialog.ui \
    ui/detectionzonedialog.ui \
    ui/aboutdialog.ui \
    ui/detectionsettingsdialog.ui \
    ui/devicecontrolwidget.ui

LIBS +=`pkg-config opencv --cflags --libs`

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
