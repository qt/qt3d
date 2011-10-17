TEMPLATE = app
TARGET = penguin_advanced
DESTDIR = $$QT.qt3d.bins/qt3d/tutorials
CONFIG += qt warn_on
QT += qt3d
SOURCES = modelview.cpp \
    main.cpp
HEADERS = modelview.h
RESOURCES = model_advanced.qrc
