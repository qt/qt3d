QT.3dquick.VERSION = 5.0.0
QT.3dquick.MAJOR_VERSION = 5
QT.3dquick.MINOR_VERSION = 0
QT.3dquick.PATCH_VERSION = 0

QT.3dquick.name = Qt3DQuick
QT.3dquick.bins = $$QT_MODULE_BIN_BASE
QT.3dquick.includes = $$QT_MODULE_INCLUDE_BASE $$QT_MODULE_INCLUDE_BASE/Qt3DQuick
QT.3dquick.private_includes = $$QT_MODULE_INCLUDE_BASE/QtSensors/$$QT.quick3d.VERSION
QT.3dquick.sources = $$QT_MODULE_BASE/src/quick3d
QT.3dquick.libs = $$QT_MODULE_LIB_BASE
QT.3dquick.plugins = $$QT_MODULE_PLUGIN_BASE
QT.3dquick.imports = $$QT_MODULE_IMPORT_BASE
QT.3dquick.depends = gui quick declarative network
QT.3dquick.DEFINES = QT_QT3DQUICK_LIB

QT_CONFIG += 3dquick
