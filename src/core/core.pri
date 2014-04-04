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

HEADERS += \
    $$PWD/qt3dcore_global.h \
    $$PWD/window.h \
    $$PWD/qtickclock.h \
    $$PWD/qscheduler.h \
    $$PWD/cameracontroller.h \
    $$PWD/qchangearbiter.h

SOURCES += \
    $$PWD/window.cpp \
    $$PWD/qtickclock.cpp \
    $$PWD/qscheduler.cpp \
    $$PWD/cameracontroller.cpp \
    $$PWD/qchangearbiter.cpp
