mt {
    desktopfile.files = $${TARGET}.qmlproject info.json
    desktopfile.path = /opt/mt/applications/$$TARGET
    icon.files = $$ICON_FILE
    icon.path = /opt/mt/applications/$$TARGET
    INSTALLS += desktopfile icon
}

!package: DESTDIR = ../../../bin

!isEmpty(QML_INFRA_FILES) {
    # rules to copy files from the *base level* of $$PWD/qml into the right place
    package {
        copyqmlinfra_install.files = $$QML_INFRA_FILES
        mt {
            copyqmlwrap.files = mt.qml
            copyqmlwrap.path = /opt/mt/applications/$$TARGET
            INSTALLS += copyqmlwrap
            copyqmlinfra_install.path = /opt/mt/applications/$$TARGET/qml
        } else {
            macx {
                copyqmlinfra_install.path = $$DESTDIR/$${TARGET}.app/Contents/Resources/qml
            } else {
                copyqmlinfra_install.path = $$DESTDIR/resources/$$CATEGORY/$${TARGET}/qml
            }
        }
        INSTALLS += copyqmlinfra_install
    } else {
        macx {
            target_dir = $$DESTDIR/$${TARGET}.app/Contents/Resources/qml
        } else {
            target_dir = $$DESTDIR/resources/$$CATEGORY/$${TARGET}/qml
        }
        copyqmlinfra.input = QML_INFRA_FILES
        copyqmlinfra.output = $$target_dir/${QMAKE_FILE_IN_BASE}${QMAKE_FILE_EXT}
        copyqmlinfra.commands = $(MKDIR) $$target_dir && $$QMAKE_COPY ${QMAKE_FILE_IN} ${QMAKE_FILE_OUT}
        copyqmlinfra.CONFIG += no_link_no_clean
        copyqmlinfra.variable_out = POST_TARGETDEPS
        QMAKE_EXTRA_COMPILERS += copyqmlinfra
    }
}

!isEmpty(QML_MESHES_FILES) {
    # rules to copy files from the *base level* of $$PWD/qml/meshes into the right place
    package {
        copyqmlmeshes_install.files = $$QML_MESHES_FILES
        mt {
            copyqmlmeshes_install.path = /opt/mt/applications/$$TARGET/qml/meshes
        } else {
            macx {
                copyqmlmeshes_install.path = $$DESTDIR/$${TARGET}.app/Contents/Resources/qml/meshes
            } else {
                copyqmlmeshes_install.path = $$DESTDIR/resources/$$CATEGORY/$${TARGET}/qml/meshes
            }
        }
        INSTALLS += copyqmlmeshes_install
    } else {
        macx {
            target_dir = $$DESTDIR/$${TARGET}.app/Contents/Resources/qml/meshes
        } else {
            target_dir = $$DESTDIR/resources/$$CATEGORY/$${TARGET}/qml/meshes
        }
        copyqmlmeshes.input = QML_MESHES_FILES
        copyqmlmeshes.output = $$target_dir/${QMAKE_FILE_IN_BASE}${QMAKE_FILE_EXT}
        copyqmlmeshes.commands = $(MKDIR) $$target_dir && $$QMAKE_COPY ${QMAKE_FILE_IN} ${QMAKE_FILE_OUT}
        copyqmlmeshes.CONFIG += no_link_no_cle:qan
        copyqmlmeshes.variable_out = POST_TARGETDEPS
        QMAKE_EXTRA_COMPILERS += copyqmlmeshes
    }
}
