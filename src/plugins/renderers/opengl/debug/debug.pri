INCLUDEPATH += $$PWD

include($$QT3D_ROOT/src/3rdparty/imgui/imgui.pri)

HEADERS += \
    $$PWD/imguirenderer_p.h

SOURCES += \
    $$PWD/imguirenderer.cpp
