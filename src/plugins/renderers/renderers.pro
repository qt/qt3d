TEMPLATE = subdirs

# OpenGL renderer
include($$OUT_PWD/../../render/qt3drender-config.pri)
QT_FOR_CONFIG += 3drender-private

#SUBDIRS += dummy

qtConfig(qt3d-opengl-renderer): SUBDIRS += opengl

qtConfig(qt3d-rhi-renderer): {
    qtHaveModule(shadertools): SUBDIRS += rhi
}
