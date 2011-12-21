QT.qt3dquick.VERSION = 5.0.0
QT.qt3dquick.MAJOR_VERSION = 5
QT.qt3dquick.MINOR_VERSION = 0
QT.qt3dquick.PATCH_VERSION = 0

QT.qt3dquick.name = Qt3DQuick
QT.qt3dquick.bins = $$QT_MODULE_BIN_BASE
QT.qt3dquick.includes = $$QT_MODULE_INCLUDE_BASE $$QT_MODULE_INCLUDE_BASE/Qt3DQuick
QT.qt3dquick.private_includes = $$QT_MODULE_INCLUDE_BASE/QtSensors/$$QT.quick3d.VERSION
QT.qt3dquick.sources = $$QT_MODULE_BASE/src/quick3d
QT.qt3dquick.libs = $$QT_MODULE_LIB_BASE
QT.qt3dquick.plugins = $$QT_MODULE_PLUGIN_BASE
QT.qt3dquick.imports = $$QT_MODULE_IMPORT_BASE
QT.qt3dquick.depends = gui quick declarative network
QT.qt3dquick.DEFINES = QT_QT3DQUICK_LIB

QT_CONFIG += qt3dquick
