TEMPLATE = lib
TARGET  = qshapesqmlplugin
CONFIG += qt plugin

QT += qt3d quick3d

DESTDIR = $$QT.quick3d.imports/Qt3D/Shapes
target.path = $$[QT_INSTALL_IMPORTS]/Qt3D/Shapes
INSTALLS += target

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

copyqmlinfra.input = QML_INFRA_FILES
copyqmlinfra.output = $$target.path/${QMAKE_FILE_IN_BASE}${QMAKE_FILE_EXT}
copyqmlinfra.commands = $$QMAKE_COPY ${QMAKE_FILE_IN} ${QMAKE_FILE_OUT}
copyqmlinfra.CONFIG += no_link_no_clean
copyqmlinfra.variable_out = PRE_TARGETDEPS
QMAKE_EXTRA_COMPILERS += copyqmlinfra

OTHER_FILES += \
    README.plugins_types \
    README.library_xml
