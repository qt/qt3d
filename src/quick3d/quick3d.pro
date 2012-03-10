load(qt_module)

TARGET     = Qt3DQuick
MODULE     = 3dquick
QT         = core gui network qml quick 3d

CONFIG += module
MODULE_PRI = ../../modules/qt_qt3dquick.pri

load(qt_module_config)

gcov {
    CONFIG += staticlib warn_on
    QMAKE_CXXFLAGS += -fprofile-arcs -ftest-coverage
    QMAKE_LFLAGS += -fprofile-arcs -ftest-coverage
} else {
    CONFIG += dll warn_on
}

include(quick3d.pri)

PUBLIC_HEADERS = $$HEADERS
HEADERS += $$PRIVATE_HEADERS
DEFINES += QT_BUILD_QT3D_QUICK_LIB

!contains(QT_CONFIG, egl):DEFINES += QT_NO_EGL






# ========== install additional files ==========

QML_FILES = \
    Cube.qml \
    Sphere.qml \
    Quad.qml \
    Teapot.qml \
    Capsule.qml \
    Cylinder.qml

QML_INFRA_FILES += \
    $$QML_FILES \
    qmldir \
    library.xml \
    plugins.qmltypes \
    cube.obj \
    quad.obj \
    teapot.bez
    # see the file README.library_xml for more on library.xml

package {
    copyqmlinfra_install.files = $$QML_INFRA_FILES
    copyqmlinfra_install.path = $$[QT_INSTALL_IMPORTS]/Qt3D/Shapes
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

OTHER_FILES += $$QML_FILES
