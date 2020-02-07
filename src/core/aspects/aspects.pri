SOURCES += \
        $$PWD/qabstractaspect.cpp \
        $$PWD/qaspectengine.cpp \
        $$PWD/qaspectfactory.cpp \
        $$PWD/qaspectmanager.cpp \
        $$PWD/aspectcommanddebugger.cpp

HEADERS += \
        $$PWD/qabstractaspect.h \
        $$PWD/qaspectengine.h \
        $$PWD/qabstractaspect_p.h \
        $$PWD/qaspectengine_p.h \
        $$PWD/qaspectfactory_p.h \
        $$PWD/qaspectmanager_p.h \
        $$PWD/aspectcommanddebugger_p.h

INCLUDEPATH += $$PWD

include($$QT3D_BUILD_ROOT/src/core/qt3dcore-config.pri)
QT_FOR_CONFIG += 3dcore-private
