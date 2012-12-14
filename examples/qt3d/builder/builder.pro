TEMPLATE = app
TARGET = builder

QT += 3d

include(../../../pkg.pri)

SOURCES = builder.cpp \
    main.cpp
HEADERS = builder.h
RESOURCES += builder.qrc

OTHER_FILES += \
    builder.rc

RC_FILE = builder.rc
