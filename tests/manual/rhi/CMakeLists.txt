# Generated from rhi.pro.

#####################################################################
## rhi Binary:
#####################################################################

qt_add_manual_test(rhi
    GUI
    SOURCES
        main.cpp
    PUBLIC_LIBRARIES
        Qt::3DCore
        Qt::3DExtras
        Qt::3DInput
        Qt::3DRender
        Qt::Gui
)

# Resources:
set(qmake_immediate_resource_files
    "qtlogo.png"
)

qt_add_resource(rhi "qmake_immediate"
    PREFIX
        "/"
    FILES
        ${qmake_immediate_resource_files}
)


## Scopes:
#####################################################################
