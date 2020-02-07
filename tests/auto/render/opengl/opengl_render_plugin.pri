# Found no way of having the test runner include the correct
# library path as runtime

#PLUGIN_SRC_PATH = $$PWD/../../../../src/plugins/renderers/opengl

#INCLUDEPATH += \
#    $$PLUGIN_SRC_PATH/graphicshelpers \
#    $$PLUGIN_SRC_PATH/io \
#    $$PLUGIN_SRC_PATH/jobs \
#    $$PLUGIN_SRC_PATH/managers \
#    $$PLUGIN_SRC_PATH/renderer \
#    $$PLUGIN_SRC_PATH/renderstates \
#    $$PLUGIN_SRC_PATH/textures

#LIBS += -L$$[QT_INSTALL_PLUGINS]/renderers/ -lopenglrenderer

# Instead we link against the sources directly
include(../../../../src/plugins/renderers/opengl/opengl.pri)
