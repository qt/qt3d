TEMPLATE = subdirs
QT_FOR_CONFIG += 3dcore-private
qtConfig(assimp):qtConfig(commandlineparser):!cross_compile: {
    SUBDIRS += qgltf
}
