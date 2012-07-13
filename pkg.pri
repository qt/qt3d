# This file is an interim measure until deployment becomes more clear for
# the Qt5 SDK and Qt3D.  Read this file in conjunction with the logic in
# qmlres.cpp.  Once Qt5 is stable and deployment is clear this will be
# removed.  Also check the README file.

# package the binary wrapper that launches the QML
testcase|qmltestcase: \
    target.path = $$[QT_INSTALL_TESTS]/$$TARGET
else: \
    target.path = $$[QT_INSTALL_EXAMPLES]/qt3d
INSTALLS += target

macx: \
    resource_dir = $${TARGET}.app/Contents/Resources
else: \
    resource_dir = resources/$$CATEGORY/$${TARGET}

DESTDIR = $$shadowed($$PWD)/bin

#  The QML_INFRA_FILES and QML_MESHES_FILES are both about QML based
# applications, so we'll install them into QT_INSTALL_DATA instead of
# QT_INSTALL_BINS
# QML_INFRA_FILES is used by our quick3d demos and examples to indicate files
# that are part of the application and should be installed (e.g. qml files,
# images, meshes etc).
# This conditional serves two purposes:
# 1) Set up a qmake extra compiler to copy relevant QML files at build time
#    to allow for a normal "change, make, test" developement cycle
# 2) Set up appropriate install paths on the same files to use "make install"
#    for building packages
!isEmpty(QML_INFRA_FILES) {

    # rules to copy files from the *base level* of $$PWD/qml into the right place
    copyqmlinfra_install.files = $$QML_INFRA_FILES
    copyqmlinfra_install.path = $$target.path/$$resource_dir/qml
    INSTALLS += copyqmlinfra_install

    # put all our demos/examples and supporting files into $BUILD_DIR/bin
    target_dir = $$DESTDIR/$$resource_dir/qml
    # create extra qmake compiler to copy files across during build step
    copyqmlinfra.input = QML_INFRA_FILES
    copyqmlinfra.output = $$target_dir/${QMAKE_FILE_IN_BASE}${QMAKE_FILE_EXT}
    copyqmlinfra.commands = $$QMAKE_COPY ${QMAKE_FILE_IN} ${QMAKE_FILE_OUT}
    copyqmlinfra.CONFIG += no_link no_clean
    copyqmlinfra.variable_out = POST_TARGETDEPS
    QMAKE_EXTRA_COMPILERS += copyqmlinfra
}

!isEmpty(QML_MESHES_FILES) {

    # rules to copy files from the *base level* of $$PWD/qml/meshes into the right place
    copyqmlmeshes_install.files = $$QML_MESHES_FILES
    copyqmlmeshes_install.path = $$target.path/$$resource_dir/qml/meshes
    INSTALLS += copyqmlmeshes_install

    target_dir = $$DESTDIR/$$resource_dir/qml/meshes
    copyqmlmeshes.input = QML_MESHES_FILES
    copyqmlmeshes.output = $$target_dir/${QMAKE_FILE_IN_BASE}${QMAKE_FILE_EXT}
    copyqmlmeshes.commands = $$QMAKE_COPY ${QMAKE_FILE_IN} ${QMAKE_FILE_OUT}
    copyqmlmeshes.CONFIG += no_link no_clean
    copyqmlmeshes.variable_out = POST_TARGETDEPS
    QMAKE_EXTRA_COMPILERS += copyqmlmeshes
}
