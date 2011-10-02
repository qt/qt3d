TEMPLATE = lib
TARGET = Qt3DQuick$${QT_LIBINFIX}
gcov {
    CONFIG += staticlib warn_on
    QMAKE_CXXFLAGS += -fprofile-arcs -ftest-coverage
    QMAKE_LFLAGS += -fprofile-arcs -ftest-coverage
} else {
    CONFIG += dll warn_on
}

QT += declarative

include(../../pkg.pri)

# See the README in the root dir re this code
package {
    macx:CONFIG(qt_framework, qt_framework|qt_no_framework) {
        LIBS += -framework Qt3D -F../threed
        INCLUDEPATH += ../threed/Qt3D.framework/Versions/1/Headers
    } else {
        win32 {
            CONFIG(debug, debug|release) {
                LIBS += ..\\threed\\debug\\Qt3Dd.lib
            } else {
                LIBS += ..\\threed\\release\\Qt3D.lib
            }
        } else {
            LIBS += -L../threed -lQt3D
        }
        INCLUDEPATH += ../../include/Qt3D
    }
    win32 {
        target.path = $$QT3D_INSTALL_BINS
    } else {
        target.path = $$QT3D_INSTALL_LIBS
    }
    INSTALLS += target
    QT += opengl network
} else {
    CONFIG += qt3d
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

include(quick3d.pri)

PUBLIC_HEADERS = $$HEADERS
HEADERS += $$PRIVATE_HEADERS
DEFINES += QT_BUILD_QT3D_QUICK_LIB

!contains(QT_CONFIG, egl):DEFINES += QT_NO_EGL

package: !qtc_harmattan {
    distInstalls.files = $$PUBLIC_HEADERS
    distInstalls.path = $$[QT_INSTALL_HEADERS]/Qt3DQuick
    INSTALLS += distInstalls
}

# If Qt has been configured to build frameworks, then the build witll put
# the Qt3DQuick library into a framework bundle, so put the headers in the bundle
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
    exportHeaders.input = PUBLIC_HEADERS
    package {
        exportHeaders.output = ../../include/Qt3DQuick/${QMAKE_FILE_IN_BASE}${QMAKE_FILE_EXT}
    } else {
        exportHeaders.output = $$[QT_INSTALL_HEADERS]/Qt3DQuick/${QMAKE_FILE_IN_BASE}${QMAKE_FILE_EXT}
    }
    exportHeaders.commands = $$QMAKE_COPY ${QMAKE_FILE_IN} ${QMAKE_FILE_OUT}
    exportHeaders.CONFIG += no_link_no_clean
    exportHeaders.variable_out = PRE_TARGETDEPS
    QMAKE_EXTRA_COMPILERS += exportHeaders
}

