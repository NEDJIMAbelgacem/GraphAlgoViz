#-------------------------------------------------
#
# Project created by QtCreator 2019-11-07T03:57:39
#
#-------------------------------------------------

QT       += widgets core gui qml quick quickcontrols2 quickwidgets

greaterThan(QT_MAJOR_VERSION, 4):

TARGET = GraphAlogViz
TEMPLATE = app

#CONFIG      += qt warn_on thread c++14

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

include(./QuickQanava/src/quickqanava.pri)


SOURCES += \
        main.cpp \
    applicationmanager.cpp \
    graph.cpp \
    customnode.cpp \
    customgraph.cpp

HEADERS += \
    applicationmanager.h \
    graph.h \
    customnode.h \
    customgraph.h

FORMS += \
        mainwindow.ui

OTHER_FILES +=  app.qml \
                custom.qml

RESOURCES   +=  cpp_sample.qrc

DISTFILES += \
    custom.qml \
    app.qml \
    directed_graph.qml \
    undirected_graph.qml \
    edge_weight.qml

