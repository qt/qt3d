# Manages deploying the QML files and resources along with the binary

distInstalls.files = qml
distInstalls.path = $$QT3D_INSTALL_DATA/quick3d/examples/$$TARGET
INSTALLS += distInstalls

win32 {
    QMAKE_DEL_FILE = del /q
    QMAKE_DEL_DIR = rmdir /s /q
}

qmlResources.files = qml
symbian {
    DEPLOYMENT += qmlResources
    qmlDeployment.sources = qml\*
    qmlDeployment.path = qml
    DEPLOYMENT += qmlDeployment
} else {
    macx {
        qmlResources.path = Contents/Resources
        QMAKE_BUNDLE_DATA += qmlResources
    } else {
        !package {
            qmlResources.input = qmlResources.files
            qmlResources.output = $$OUT_PWD/../../../bin/resources/examples/$$TARGET/qml
            qmlResources.commands = $$QMAKE_COPY_DIR ${QMAKE_FILE_IN} ${QMAKE_FILE_OUT}
            qmlResources.CONFIG += no_link_no_clean
            qmlResources.variable_out = POST_TARGETDEPS
            QMAKE_EXTRA_COMPILERS += qmlResources
        }
    }
}
