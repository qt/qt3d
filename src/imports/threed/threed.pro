TEMPLATE = lib
TARGET  = qthreedqmlplugin
CONFIG += qt plugin

QT += quick qml 3d 3dquick

DESTDIR = $$QT.3dquick.imports/Qt3D
target.path = $$[QT_INSTALL_IMPORTS]/Qt3D
INSTALLS += target

SOURCES += \
    threed.cpp \
    qt3dnamespace.cpp \
    scale3d.cpp \
    viewport.cpp \
    qgraphicslookattransform.cpp \
    shaderprogram.cpp \
    skybox.cpp \
    billboarditem3d.cpp

HEADERS += \
    qt3dnamespace.h \
    scale3d.h \
    viewport.h \
    qgraphicslookattransform.h \
    shaderprogram.h \
    shaderprogram_p.h \
    skybox.h \
    billboarditem3d.h

QML_INFRA_FILES = \
    qmldir \
    library.xml \
    plugins.qmltypes

package {
    copyqmlinfra_install.files = $$QML_INFRA_FILES
    copyqmlinfra_install.path = $$[QT_INSTALL_IMPORTS]/Qt3D
    INSTALLS += copyqmlinfra_install
} else {
    copyqmlinfra.input = QML_INFRA_FILES
    copyqmlinfra.output = $$[QT_INSTALL_IMPORTS]/Qt3D/${QMAKE_FILE_IN_BASE}${QMAKE_FILE_EXT}
    copyqmlinfra.commands = $$QMAKE_COPY ${QMAKE_FILE_IN} ${QMAKE_FILE_OUT}
    copyqmlinfra.CONFIG += no_link_no_clean
    copyqmlinfra.variable_out = PRE_TARGETDEPS
    QMAKE_EXTRA_COMPILERS += copyqmlinfra
}

OTHER_FILES += \
    README.plugins_types \
    README.library_xml


