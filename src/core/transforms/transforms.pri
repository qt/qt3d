SOURCES += \
    $$PWD/qtransform.cpp \
    $$PWD/qjoint.cpp \
    $$PWD/qabstractskeleton.cpp \
    $$PWD/qskeleton.cpp \
    $$PWD/qskeletonloader.cpp \
    $$PWD/qarmature.cpp

HEADERS += \
    $$PWD/qtransform.h \
    $$PWD/qtransform_p.h \
    $$PWD/qmath3d_p.h \
    $$PWD/qjoint.h \
    $$PWD/qjoint_p.h \
    $$PWD/qabstractskeleton.h \
    $$PWD/qabstractskeleton_p.h \
    $$PWD/qskeleton.h \
    $$PWD/qskeleton_p.h \
    $$PWD/qskeletonloader.h \
    $$PWD/qskeletonloader_p.h \
    $$PWD/qarmature.h \
    $$PWD/qarmature_p.h \
    $$PWD/vector4d_p.h \
    $$PWD/vector3d_p.h

INCLUDEPATH += $$PWD

qtConfig(qt3d-simd-sse2) {
    CONFIG += simd

    SSE2_HEADERS += \
            $$PWD/vector4d_sse_p.h \
            $$PWD/vector3d_sse_p.h

    SSE2_SOURCES += \
            $$PWD/vector4d_sse.cpp \
            $$PWD/vector3d_sse.cpp
}
