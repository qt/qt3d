# This file is an interim measure until deployment becomes more clear for
# the Qt5 SDK and Qt3D.  Read this file in conjunction with the logic in
# qmlres.cpp.  Once Qt5 is stable and deployment is clear this will be
# removed.  Also check the README file.

package {
    # default install path for packaging qml resources - package case
    resource_dir = $$[QT_INSTALL_DATA]/qt3d/resources/$$CATEGORY/$${TARGET}
} else {
    # if we're not packaging, build all our demos/examples/etc and supporting files
    # into $BUILD_DIR/bin
    DESTDIR = ../../../bin

    # default install path for packaging qml resources - developer case
    resource_dir = $$DESTDIR/resources/$$CATEGORY/$${TARGET}
}

# package the binary wrapper that launches the QML
package {
    macx: resource_dir = $$[QT_INSTALL_DATA]/qt3d/$${TARGET}.app/Contents/Resources
    target.path = $$[QT_INSTALL_DATA]/qt3d
    INSTALLS += target
} else {
    macx: resource_dir = $$DESTDIR/$$QMAKE_BUNDLE/Contents/Resources/qml
}

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
    package {
        copyqmlinfra_install.files = $$QML_INFRA_FILES
        mt: copyqmlinfra_install.files += info.json mt.qml $$ICON_FILE
        copyqmlinfra_install.path = $$resource_dir/qml
        INSTALLS += copyqmlinfra_install
    } else {
        # if we're not packaging, put all our demos/examples and supporting files
        # into $BUILD_DIR/bin
        target_dir = $$resource_dir/qml
        target_dir ~= s,/,$$QMAKE_DIR_SEP,
        # create extra qmake compiler to copy files across during build step
        copyqmlinfra.input = QML_INFRA_FILES
        copyqmlinfra.output = $$target_dir/${QMAKE_FILE_IN_BASE}${QMAKE_FILE_EXT}
        win32: isEmpty(QMAKE_SH) {
            # If we are in windows, and not a mingw shell, then the mkdir binary does not handle
            # the -p switch, and will fail if the directory already exists, so make it subject to
            # an "exists" test.  The parens are necessary otherwise the copy won't occur when the
            # test fails, since $$QMAKE_CHK_DIR_EXISTS is "IF NOT EXISTS"
            copyqmlinfra.commands = ($$QMAKE_CHK_DIR_EXISTS $$target_dir $$QMAKE_MKDIR $$target_dir) && $$QMAKE_COPY ${QMAKE_FILE_IN} ${QMAKE_FILE_OUT}
        } else {
            # in mac, linux, and windows-with-mingw $$QMAKE_MKDIR has -p so this will always work
            copyqmlinfra.commands = $$QMAKE_MKDIR $$target_dir && $$QMAKE_COPY ${QMAKE_FILE_IN} ${QMAKE_FILE_OUT}
        }
        copyqmlinfra.CONFIG += no_link_no_clean
        copyqmlinfra.variable_out = POST_TARGETDEPS
        QMAKE_EXTRA_COMPILERS += copyqmlinfra
    }
}

!isEmpty(QML_MESHES_FILES) {
    # rules to copy files from the *base level* of $$PWD/qml/meshes into the right place
    package {
        copyqmlmeshes_install.files = $$QML_MESHES_FILES
        copyqmlmeshes_install.path = $$resource_dir/qml/meshes
        INSTALLS += copyqmlmeshes_install
    } else {
        target_dir = $$resource_dir/qml/meshes
        target_dir ~= s,/,$$QMAKE_DIR_SEP,
        copyqmlmeshes.input = QML_MESHES_FILES
        copyqmlmeshes.output = $$target_dir/${QMAKE_FILE_IN_BASE}${QMAKE_FILE_EXT}
        win32: isEmpty(QMAKE_SH) {
            # If we are in windows, and not a mingw shell, then the mkdir binary does not handle
            # the -p switch, and will fail if the directory already exists, so make it subject to
            # an "exists" test.  The parens are necessary otherwise the copy won't occur when the
            # test fails, since $$QMAKE_CHK_DIR_EXISTS is "IF NOT EXISTS"
            copyqmlmeshes.commands = ($$QMAKE_CHK_DIR_EXISTS $$target_dir $$QMAKE_MKDIR $$target_dir) && $$QMAKE_COPY ${QMAKE_FILE_IN} ${QMAKE_FILE_OUT}
        } else {
            # in mac, linux, and windows-with-mingw $$QMAKE_MKDIR has -p so this will always work
            copyqmlmeshes.commands = $$QMAKE_MKDIR $$target_dir && $$QMAKE_COPY ${QMAKE_FILE_IN} ${QMAKE_FILE_OUT}
        }
        copyqmlmeshes.CONFIG += no_link_no_clean
        copyqmlmeshes.variable_out = POST_TARGETDEPS
        QMAKE_EXTRA_COMPILERS += copyqmlmeshes
    }
}
