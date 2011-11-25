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

include(../../pkg.pri)

package {
    win32 {
        target.path = $$QT3D_INSTALL_BINS
    } else {
        target.path = $$QT3D_INSTALL_LIBS
    }
    INSTALLS += target
} else {
    DESTDIR = $$QT3D_INSTALL_LIBS
}

win32 {
    !static:DEFINES += QT_MAKEDLL
    package {
        installDll.path = $$QT3D_INSTALL_BINS
        installDll.files = $$DESTDIR_TARGET
        INSTALLS += installDll
    } else {
        DLLDESTDIR = $$QT3D_INSTALL_BINS
    }
    CONFIG(debug, debug|release) {
        TARGET = $$member(TARGET, 0)d
    }
}

symbian {
    DEFINES += QT_MAKEDLL
    CONFIG += epocallowdlldata
    MMP_RULES += EXPORTUNFROZEN
    TARGET.CAPABILITY = All -Tcb
    TARGET.UID3 = 0x20031E95
}

include(../private/private.pri)
include(threed.pri)
PUBLIC_HEADERS = $$HEADERS
HEADERS += $$PRIVATE_HEADERS
DEFINES += QT_BUILD_QT3D_LIB

!contains(QT_CONFIG, egl):DEFINES += QT_NO_EGL

package {
    distInstalls.files = $$PUBLIC_HEADERS
    distInstalls.path = $$QT3D_INSTALL_HEADERS/Qt3D
    INSTALLS += distInstalls
}

# If Qt has been configured to build frameworks, then the build will put
# the Qt3D library into a framework bundle, so put the headers in the bundle
# as well.  Other OS's, or mac without frameworks, install the headers into
# the Qt build tree directly.
macx:CONFIG(qt_framework, qt_framework|qt_no_framework) {
    QMAKE_LFLAGS_SONAME = -Wl,-install_name,$$DESTDIR/
    CONFIG += lib_bundle
    FRAMEWORK_HEADERS.version = Versions
    FRAMEWORK_HEADERS.path = Headers
    FRAMEWORK_HEADERS.files = $$PUBLIC_HEADERS
    QMAKE_BUNDLE_DATA += FRAMEWORK_HEADERS
} else {
    !symbian {
        exportHeaders.input = PUBLIC_HEADERS
        # in the package case we need the headers for the actual build, the
        # distInstalls stanza above takes care of packaging the headers for the dev pkg
        package {
            exportHeaders.output = ../../include/Qt3D/${QMAKE_FILE_IN_BASE}${QMAKE_FILE_EXT}
        } else {
            !maemo: exportHeaders.output = $$[QT_INSTALL_HEADERS]/Qt3D/${QMAKE_FILE_IN_BASE}${QMAKE_FILE_EXT}
        }
        exportHeaders.commands = $$QMAKE_COPY ${QMAKE_FILE_IN} ${QMAKE_FILE_OUT}
        exportHeaders.CONFIG += no_link_no_clean
        exportHeaders.variable_out = PRE_TARGETDEPS
        QMAKE_EXTRA_COMPILERS += exportHeaders
    }
}

symbian {
    export_headers.files = $$PUBLIC_HEADERS
    for(export_header, export_headers.files) {
        BLD_INF_RULES.prj_exports += "$$export_header $$MW_LAYER_PUBLIC_EXPORT_PATH(Qt3D/$$basename(export_header))"
    }
}
