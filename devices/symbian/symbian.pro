# Use subdirs template to suppress generation of unnecessary files
TEMPLATE = subdirs

symbian {
    load(data_caging_paths)

    SUBDIRS=
    # WARNING: Changing TARGET name will break Symbian SISX upgrade functionality
    # DO NOT TOUCH TARGET VARIABLE IF YOU ARE NOT SURE WHAT YOU ARE DOING
    TARGET = "Qt Quick 3D"

    # This is the UID for the overall .SIS file package
    TARGET.UID3 = 0x20031E9A

    VERSION = 1.0.0

    vendorinfo = \
        "; Localised Vendor name" \
        "%{\"Nokia\"}" \
        " " \
        "; Unique Vendor name" \
        ":\"Nokia\"" \
        " "
    qt3dlibraries.pkg_prerules += vendorinfo

    qt3dlibraries.sources = Qt3D$${QT_LIBINFIX}.dll
    qt3dlibraries.path = /sys/bin
    DEPLOYMENT += qt3dlibraries

    contains(CONFIG, old_importer): {
        qt3dsceneformats_plugins.sources = \
                                    qscene3ds$${QT_LIBINFIX}.dll \
                                    qsceneobj$${QT_LIBINFIX}.dll \
                                    qscenebezier$${QT_LIBINFIX}.dll
        qt3dsceneformats_plugins.path = $$QT_PLUGINS_BASE_DIR/sceneformats
        DEPLOYMENT += qt3dsceneformats_plugins
    } else {
        qt3dsceneformats_plugins.sources = \
                                    qsceneai$${QT_LIBINFIX}.dll \
                                    qscenebezier$${QT_LIBINFIX}.dll
        qt3dsceneformats_plugins.path = $$QT_PLUGINS_BASE_DIR/sceneformats
        DEPLOYMENT += qt3dsceneformats_plugins
    }

    qt3dquick.sources = Qt3DQuick$${QT_LIBINFIX}.dll
    qt3dquick.path = /sys/bin
    qt3dthreedImports.sources = \
                            qthreedqmlplugin.dll \
                            $$PWD/../../src/imports/threed/qmldir
    qt3dthreedImports.path = $$QT_IMPORTS_BASE_DIR/Qt3D
    qt3dshapesImports.sources = \
                            qshapesqmlplugin.dll \
                            $$PWD/../../src/imports/shapes/Cube.qml \
                            $$PWD/../../src/imports/shapes/cube.obj \
                            $$PWD/../../src/imports/shapes/Quad.qml \
                            $$PWD/../../src/imports/shapes/quad.obj \
                            $$PWD/../../src/imports/shapes/Teapot.qml \
                            $$PWD/../../src/imports/shapes/teapot.bez \
                            $$PWD/../../src/imports/shapes/qmldir
    qt3dshapesImports.path = $$QT_IMPORTS_BASE_DIR/Qt3D/Shapes
    DEPLOYMENT += qt3dquick qt3dthreedImports qt3dshapesImports
}
