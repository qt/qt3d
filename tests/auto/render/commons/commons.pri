SOURCES += \
    $$PWD/testrenderer.cpp

HEADERS += \
    $$PWD/testrenderer.h

useCommonTestAspect {
  SOURCES += $$PWD/testaspect.cpp
  HEADERS += $$PWD/testaspect.h
}

INCLUDEPATH += $$PWD
CONFIG += c++17

QT += core-private 3dcore 3dcore-private 3drender 3drender-private
