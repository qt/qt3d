INCLUDEPATH += $$PWD
VPATH += $$PWD
HEADERS += geometry/qglcube.h \
    geometry/qglsphere.h \
    geometry/qgeometrydata.h \
    geometry/qlogicalvertex.h \
    geometry/qglbuilder.h \
    geometry/qglbezierpatches.h \
    geometry/qglmaterialcollection.h \
    geometry/qglteapot.h \
    geometry/qglcylinder.h \
    geometry/qgldome.h
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
