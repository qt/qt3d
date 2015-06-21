
SOURCES += \
    $$PWD/qaspectjob.cpp \
    $$PWD/qaspectjobmanager.cpp \
    $$PWD/qabstractaspectjobmanager.cpp \
    $$PWD/qthreadpooler.cpp \
    $$PWD/task.cpp \
    $$PWD/dependencyhandler.cpp

# TODO: Make tasking API configurable
#use_thread_weaver {
#SOURCES += \
#    $$PWD/weaverjob.cpp
#
#HEADERS += \
#    $$PWD/weaverjob_p.h \
#}

HEADERS += \
    $$PWD/qaspectjob.h \
    $$PWD/qaspectjob_p.h \
    $$PWD/qaspectjobproviderinterface.h \
    $$PWD/qaspectjobmanager_p.h \
    $$PWD/qabstractaspectjobmanager_p.h \
    $$PWD/dependencyhandler_p.h \
    $$PWD/task_p.h \
    $$PWD/qthreadpooler_p.h

INCLUDEPATH += $$PWD
