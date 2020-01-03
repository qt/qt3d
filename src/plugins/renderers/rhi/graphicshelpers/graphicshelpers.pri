#DEFINES += QT3D_RENDER_ASPECT_OPENGL_DEBUG

INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/graphicscontext_p.h \
    $$PWD/graphicshelperinterface_p.h \
    $$PWD/imagesubmissioncontext_p.h \
    $$PWD/submissioncontext_p.h \
    $$PWD/texturesubmissioncontext_p.h \
    $$PWD/qgraphicsutils_p.h

SOURCES += \
    $$PWD/graphicscontext.cpp \
    $$PWD/imagesubmissioncontext.cpp \
    $$PWD/submissioncontext.cpp \
    $$PWD/texturesubmissioncontext.cpp
