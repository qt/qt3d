TEMPLATE = lib
TARGET  = qshapesqmlplugin
CONFIG += qt plugin

CONFIG += qt3dquick_deploy_pkg
include(../../../pkg.pri)

# See the README in the root dir re this code
package {
    target.path += $$QT3D_INSTALL_IMPORTS/Qt3D/Shapes
    INSTALLS += target
    QT += declarative
} else {
    CONFIG += qt3dquick qt3d
    DESTDIR = $$QT3D_INSTALL_IMPORTS/Qt3D/Shapes
}

win32 {
    CONFIG(debug, debug|release) {
        TARGET = $$member(TARGET, 0)d
    }
}

symbian {
    CONFIG += epocallowdlldata
    MMP_RULES += EXPORTUNFROZEN
    LIBS += -lQt3D$${QT_LIBINFIX} -lQt3DQuick$${QT_LIBINFIX}
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = All -Tcb
    TARGET.UID3 = 0x20031E9C
}

SOURCES += \
    sphere.cpp \
    shapes.cpp \
    cylinder.cpp \
    capsule.cpp \
    line.cpp \
    point.cpp
HEADERS += \
    sphere.h \
    cylinder.h \
    capsule.h \
    line.h \
    point.h

QML_INFRA_FILES += \
    qmldir \
    library.xml \
    plugins.qmltypes \
    Cube.qml \
    cube.obj \
    Quad.qml \
    quad.obj \
    Teapot.qml \
    teapot.bez
# see the file README.library_xml for more on library.xml

package {
    copyqmlinfra_install.files = $$QML_INFRA_FILES
    copyqmlinfra_install.path = $$QT3D_INSTALL_IMPORTS/Qt3D/Shapes
    INSTALLS += copyqmlinfra_install
} else {
    copyqmlinfra.input = QML_INFRA_FILES
    copyqmlinfra.output = $$[QT_INSTALL_IMPORTS]/Qt3D/Shapes/${QMAKE_FILE_IN_BASE}${QMAKE_FILE_EXT}
    copyqmlinfra.commands = $$QMAKE_COPY ${QMAKE_FILE_IN} ${QMAKE_FILE_OUT}
    copyqmlinfra.CONFIG += no_link_no_clean
    copyqmlinfra.variable_out = PRE_TARGETDEPS
    QMAKE_EXTRA_COMPILERS += copyqmlinfra
}

OTHER_FILES += \
    README.plugins_types \
    README.library_xml
