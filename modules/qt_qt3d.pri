QT.qt3d.VERSION = 5.0.0
QT.qt3d.MAJOR_VERSION = 5
QT.qt3d.MINOR_VERSION = 0
QT.qt3d.PATCH_VERSION = 0

QT.qt3d.name = Qt3D
QT.qt3d.bins = $$QT_MODULE_BIN_BASE
QT.qt3d.includes = $$QT_MODULE_INCLUDE_BASE $$QT_MODULE_INCLUDE_BASE/Qt3D
QT.qt3d.private_includes = $$QT_MODULE_INCLUDE_BASE/Qt3D/$$QT.qt3d.VERSION
QT.qt3d.sources = $$QT_MODULE_BASE/src/threed
QT.qt3d.libs = $$QT_MODULE_LIB_BASE
QT.qt3d.plugins = $$QT_MODULE_PLUGIN_BASE
QT.qt3d.imports = $$QT_MODULE_IMPORT_BASE
QT.qt3d.depends = gui opengl
QT.qt3d.DEFINES = QT_QT3D_LIB

QT_CONFIG += qt3d
