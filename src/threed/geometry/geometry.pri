INCLUDEPATH += $$PWD
VPATH += $$PWD
HEADERS += qglcube.h \
    qglsphere.h \
    qgeometrydata.h \
    qlogicalvertex.h \
    qglbuilder.h \
    qglbezierpatches.h \
    qglmaterialcollection.h \
    qglteapot.h \
    qglcylinder.h \
    qgldome.h
SOURCES += qglcube.cpp \
    qglsphere.cpp \
    qgeometrydata.cpp \
    qglbuilder.cpp \
    qglsection.cpp \
    qglbezierpatches.cpp \
    qglmaterialcollection.cpp \
    qglteapot.cpp \
    qlogicalvertex.cpp \
    qglcylinder.cpp \
    qgldome.cpp
PRIVATE_HEADERS += qglteapot_data_p.h \
    qglbuilder_p.h \
    qglsection_p.h \
    qglteapot_data_p.h \
    qvector_utils_p.h
