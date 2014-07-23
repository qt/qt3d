INCLUDEPATH += $$PWD

# Aspects
include (./aspects/aspects.pri)
# Jobs
include (./jobs/jobs.pri)
# Nodes
include (./nodes/nodes.pri)
# Qml Components
include (./core-components/core-components.pri)
# Bounds
include (./bounds/bounds.pri)
#Transformations
include (./transforms/transforms.pri)
# Resources Management
include (./resources/resources.pri)
# IO
include (./io/io.pri)

HEADERS += \
    $$PWD/qt3dcore_global.h \
    $$PWD/window.h \
    $$PWD/qtickclock.h \
    $$PWD/qscheduler.h \
    $$PWD/cameracontroller.h \
    $$PWD/qchangearbiter.h \
    $$PWD/corelogging.h \
    $$PWD/qscenechange.h \
    $$PWD/qscenepropertychange.h \
    $$PWD/qobservableinterface.h \
    $$PWD/qobservable.h \
    $$PWD/qobserverinterface.h \
    $$PWD/qchangearbiter_p.h \
    $$PWD/qobservable_p.h \
    $$PWD/qscenechange_p.h \
    $$PWD/qscenepropertychange_p.h \
    $$PWD/qsceneobserverinterface.h

SOURCES += \
    $$PWD/window.cpp \
    $$PWD/qtickclock.cpp \
    $$PWD/qscheduler.cpp \
    $$PWD/cameracontroller.cpp \
    $$PWD/qchangearbiter.cpp \
    $$PWD/corelogging.cpp \
    $$PWD/qobservableinterface.cpp \
    $$PWD/qobservable.cpp \
    $$PWD/qobserverinterface.cpp \
    $$PWD/qscenechange.cpp \
    $$PWD/qscenepropertychange.cpp \
    $$PWD/qsceneobserverinterface.cpp
