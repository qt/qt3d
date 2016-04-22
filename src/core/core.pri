INCLUDEPATH += $$PWD

# Aspects
include (./aspects/aspects.pri)
# Jobs
include (./jobs/jobs.pri)
# Nodes
include (./nodes/nodes.pri)
#Transformations
include (./transforms/transforms.pri)
# Resources Management
include (./resources/resources.pri)
# Services
include (./services/services.pri)

HEADERS += \
    $$PWD/qt3dcore_global.h \
    $$PWD/qtickclock_p.h \
    $$PWD/qscheduler_p.h \
    $$PWD/corelogging_p.h \
    $$PWD/qscenechange.h \
    $$PWD/qscenechange_p.h \
    $$PWD/qsceneobserverinterface_p.h \
    $$PWD/qpostman_p.h \
    $$PWD/qobservableinterface_p.h \
    $$PWD/qobserverinterface_p.h \
    $$PWD/qlockableobserverinterface_p.h \
    $$PWD/qchangearbiter_p.h \
    $$PWD/qbackendnodefactory_p.h \
    $$PWD/qt3dcore_global_p.h \
    $$PWD/qscene_p.h \
    $$PWD/qnodecreatedchange_p.h \
    $$PWD/qnodecreatedchange.h \
    $$PWD/qnodedestroyedchange_p.h \
    $$PWD/qcomponentaddedchange.h \
    $$PWD/qcomponentaddedchange_p.h \
    $$PWD/qcomponentremovedchange.h \
    $$PWD/qcomponentremovedchange_p.h \
    $$PWD/qnodepropertychangebase.h \
    $$PWD/qnodepropertychangebase_p.h \
    $$PWD/qnodepropertychange_p.h \
    $$PWD/qnodepropertychange.h \
    $$PWD/qbackendnodepropertychange.h \
    $$PWD/qbackendnodepropertychange_p.h \
    $$PWD/qnodeaddedpropertychange.h \
    $$PWD/qnodeaddedpropertychange_p.h \
    $$PWD/qnoderemovedpropertychange.h \
    $$PWD/qnoderemovedpropertychange_p.h

SOURCES += \
    $$PWD/qtickclock.cpp \
    $$PWD/qscheduler.cpp \
    $$PWD/qchangearbiter.cpp \
    $$PWD/corelogging.cpp \
    $$PWD/qobservableinterface.cpp \
    $$PWD/qobserverinterface.cpp \
    $$PWD/qlockableobserverinterface.cpp \
    $$PWD/qscenechange.cpp \
    $$PWD/qsceneobserverinterface.cpp \
    $$PWD/qpostman.cpp \
    $$PWD/qscene.cpp \
    $$PWD/qbackendnodefactory.cpp \
    $$PWD/qnodecreatedchange.cpp \
    $$PWD/qnodedestroyedchange.cpp \
    $$PWD/qcomponentaddedchange.cpp \
    $$PWD/qcomponentremovedchange.cpp \
    $$PWD/qnodepropertychangebase.cpp \
    $$PWD/qnodepropertychange.cpp \
    $$PWD/qbackendnodepropertychange.cpp \
    $$PWD/qnodeaddedpropertychange.cpp \
    $$PWD/qnoderemovedpropertychange.cpp
