TEMPLATE = app
TARGET = photobrowser3d
QT += 3d

include (../../../pkg.pri)

SOURCES += main.cpp\
    photobrowser3dview.cpp \
    imagedisplay.cpp \
    skybox.cpp \
    imagemanager.cpp \
    imageloader.cpp \
    qfocusadaptor.cpp \
    thumbnailableimage.cpp \
    qatlas.cpp \
    thumbnailnode.cpp \
    thumbnaileffect.cpp \
    filescanner.cpp \
    bytereader.cpp \
    threadpool.cpp \
    buttons.cpp \
    qphotobrowser3dscene.cpp \
    pancontroller.cpp

HEADERS  += photobrowser3dview.h \
    imagedisplay.h \
    skybox.h \
    imagemanager.h \
    imageloader.h \
    qfocusadaptor.h \
    thumbnailableimage.h \
    qatlas.h \
    thumbnailnode.h \
    thumbnaileffect.h \
    filescanner.h \
    bytereader.h \
    threadpool.h \
    buttons.h \
    qphotobrowser3dscene.h \
    pancontroller.h

# Uncomment this line to force all file loading is done in the gui thread
# instead of a background thread - useful for debugging.
# DEFINES += QT_NO_THREADED_FILE_LOAD

# Uncomment this to use test images instead of scanning the file-system for
# pictures.  The test images are stored in in-memory resources "files".
# DEFINES += QT_USE_TEST_IMAGES

RESOURCES += \
    photobrowser3d.qrc

OTHER_FILES += \
    shaders/replace_texture.fsh \
    shaders/replace_texture.vsh

OTHER_FILES += \
    photobrowser.rc

RC_FILE = photobrowser.rc
