QT.quick3d.VERSION = 5.0.0
QT.quick3d.MAJOR_VERSION = 5
QT.quick3d.MINOR_VERSION = 0
QT.quick3d.PATCH_VERSION = 0

QT.quick3d.name = Qt3DQuick
QT.quick3d.bins = $$QT_MODULE_BIN_BASE
QT.quick3d.includes = $$QT_MODULE_INCLUDE_BASE $$QT_MODULE_INCLUDE_BASE/Qt3DQuick
QT.quick3d.private_includes = $$QT_MODULE_INCLUDE_BASE/QtSensors/$$QT.quick3d.VERSION
QT.quick3d.sources = $$QT_MODULE_BASE/src/quick3d
QT.quick3d.libs = $$QT_MODULE_LIB_BASE
QT.quick3d.plugins = $$QT_MODULE_PLUGIN_BASE
QT.quick3d.imports = $$QT_MODULE_IMPORT_BASE
QT.quick3d.depends = gui opengl declarative network
QT.quick3d.DEFINES = QT_QT3DQUICK_LIB

QT_CONFIG += quick3d
