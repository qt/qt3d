
INCLUDEPATH += $$PWD
VPATH += $$PWD

HEADERS += \
    materials/qglabstractmaterial.h \
    materials/qglcolormaterial.h \
    materials/qglmaterial.h \
    materials/qgltwosidedmaterial.h

SOURCES += \
    qglabstractmaterial.cpp \
    qglcolormaterial.cpp \
    qglmaterial.cpp \
    qgltwosidedmaterial.cpp

PRIVATE_HEADERS += \
    qglmaterial_p.h
