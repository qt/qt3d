TEMPLATE = lib
TARGET = Qt3D$${QT_LIBINFIX}
gcov {
    CONFIG += staticlib warn_on
    QMAKE_CXXFLAGS += -fprofile-arcs -ftest-coverage
    QMAKE_LFLAGS += -fprofile-arcs -ftest-coverage
} else {
    CONFIG += dll warn_on
}
QT += opengl \
    network
DESTDIR = $$[QT_INSTALL_LIBS]

win32 {
    DLLDESTDIR = ../../bin
    !static:DEFINES += QT_MAKEDLL

    CONFIG(debug, debug|release) {
        TARGET = $$member(TARGET, 0)d
    }
}

symbian {
    DEFINES += QT_MAKEDLL
    CONFIG += epocallowdlldata
    contains(QT_EDITION, OpenSource) {
        TARGET.CAPABILITY = LocalServices NetworkServices ReadUserData UserEnvironment WriteUserData
    } else {
        TARGET.CAPABILITY = All -Tcb
    }
}

include(../private/private.pri)
include(threed.pri)
PUBLIC_HEADERS = $$HEADERS
HEADERS += $$PRIVATE_HEADERS
DEFINES += QT_BUILD_QT3D_LIB

!contains(QT_CONFIG, egl):DEFINES += QT_NO_EGL

INSTALL_HEADERS = ""
for(hdr, PUBLIC_HEADERS) {
    found_vdir = $$PWD
    for(vdir, VPATH) {
        found_vdir = $$vdir
        exists($$found_vdir/$$hdr):break()
    }
    INSTALL_HEADERS += $$found_vdir/$$hdr
}


#      -install_name	/Users/sarasmit/build/qt/qt-qml/lib/QtOpenGL.framework/Versions/4/QtOpenGL

# If Qt has been configured to build frameworks, then the build will put
# the Qt3D library into a framework bundle, so put the headers in the bundle
# as well.  Other OS's, or mac without frameworks, install the headers into
# the Qt build tree directly.
macx:CONFIG(qt_framework, qt_framework|qt_no_framework) {
    QMAKE_LFLAGS_SONAME = -Wl,-install_name,$$DESTDIR/
    CONFIG += lib_bundle
    FRAMEWORK_HEADERS.version = Versions
    FRAMEWORK_HEADERS.path = Headers
    FRAMEWORK_HEADERS.files = $$INSTALL_HEADERS
    QMAKE_BUNDLE_DATA += FRAMEWORK_HEADERS
} else {
    exportHeaders.input = PUBLIC_HEADERS
    exportHeaders.output = $$[QT_INSTALL_HEADERS]/Qt3D/${QMAKE_FILE_IN_BASE}${QMAKE_FILE_EXT}
    exportHeaders.commands = $$QMAKE_COPY ${QMAKE_FILE_IN} ${QMAKE_FILE_OUT}
    exportHeaders.CONFIG += no_link_no_clean
    exportHeaders.variable_out = PRE_TARGETDEPS
    QMAKE_EXTRA_COMPILERS += exportHeaders
}
