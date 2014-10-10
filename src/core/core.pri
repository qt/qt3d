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
    $$PWD/corelogging.h \
    $$PWD/qscenechange.h \
    $$PWD/qscenepropertychange.h \
    $$PWD/qscenechange_p.h \
    $$PWD/qscenepropertychange_p.h \
    $$PWD/qsceneobserverinterface.h \
    $$PWD/qpostman_p.h \
    $$PWD/qscene.h \
    $$PWD/qsceneinterface.h \
    $$PWD/qbackendscenepropertychange.h \
    $$PWD/qbackendscenepropertychange_p.h \
    $$PWD/qobservable_p.h \
    $$PWD/qobservableinterface_p.h \
    $$PWD/qobserverinterface_p.h \
    $$PWD/qchangearbiter_p.h

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
    $$PWD/qsceneobserverinterface.cpp \
    $$PWD/qpostman.cpp \
    $$PWD/qscene.cpp \
    $$PWD/qbackendscenepropertychange.cpp
