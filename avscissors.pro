#-------------------------------------------------
#
# Project created by QtCreator 2018-10-11T23:20:26
#
#-------------------------------------------------

QT       += core gui multimedia multimediawidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = avscissors
TEMPLATE = app
CONFIG += c++11

OBJECTS_DIR = generated_files
MOC_DIR = generated_files
UI_DIR = generated_files

INCLUDEPATH += $$PWD/src/gui_qt

# For OpenCV.
LIBS += -lopencv_core -lopencv_imgproc -lopencv_highgui

SOURCES +=  src/main.cpp \
    src/video/video_activity.cpp \
    src/video/video_object.cpp \
    src/video/video_info.cpp \
    src/video/video_player.cpp \
    src/audio/audio_file.cpp \
    src/gui_qt/qt_main_window.cpp \
    src/messager/messager.cpp \
    src/gui_qt/qt_activity_strip.cpp \
    src/kiss_fft/kiss_fft.c

HEADERS  +=  src/common.h \
    src/types.h \
    src/video/video_player.h \
    src/video/video_info.h \
    src/video/video_activity.h \
    src/video/video_object.h \
    src/audio/audio_file.h \
    src/gui_qt/qt_main_window.h \
    src/messager/messager.h \
    src/gui_qt/qt_activity_strip.h \
    src/kiss_fft/kissfft.hh \
    src/kiss_fft/kiss_fft.h \
    src/kiss_fft/_kiss_fft_guts.h

FORMS    += \
    src/gui_qt/qt_main_window.ui

# C++. For GCC/Clang.
QMAKE_CXXFLAGS += -g
QMAKE_CXXFLAGS += -ansi
QMAKE_CXXFLAGS += -O2
QMAKE_CXXFLAGS += -Wall
QMAKE_CXXFLAGS += -pipe
QMAKE_CXXFLAGS += -pedantic
