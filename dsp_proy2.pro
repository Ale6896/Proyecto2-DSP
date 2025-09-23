## We use g++-12, for that setup the environment variable
##   export QMAKE_CXX=g++-12
## before running qmake, or inside QtCreator, set Projects > Build & Run > Build Environment
##


QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

CONFIG += c++20

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Define the dependencies
LIBS += -ljack -lsndfile -lboost_program_options -lfftw3f -lgsl -lstdc++

SOURCES += \
    freq_filter.cpp \
    jack_client.cpp \
    main.cpp \
    mainwindow.cpp \
    passthrough_client.cpp \
    qcustomplot.cpp \
    proy2_client.cpp \
    sndfile_thread.cpp \
    waitkey.cpp

HEADERS += \
    filter_type.h \
    freq_filter.h \
    jack_client.h \
    mainwindow.h \
    passthrough_client.h \
    prealloc_ringbuffer.h \
    prealloc_ringbuffer.tpp \
    proy2_client.h \
    qcustomplot.h \
    sndfile_thread.h \
    waitkey.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
