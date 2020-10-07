# Generated from animation-keyframe-programmatic.pro.

#####################################################################
## animation-keyframe-programmatic Binary:
#####################################################################

qt_internal_add_manual_test(animation-keyframe-programmatic
    GUI
    SOURCES
        animationfactory.cpp animationfactory.h
        main.cpp
    PUBLIC_LIBRARIES
        Qt::3DQuickAnimation
        Qt::3DQuickExtras
        Qt::Gui
)

# Resources:
set(main_resource_files
    "DefaultSceneEntity.qml"
    "main.qml"
)

qt_internal_add_resource(animation-keyframe-programmatic "main"
    PREFIX
        "/"
    FILES
        ${main_resource_files}
)


#### Keys ignored in scope 1:.:.:animation-keyframe-programmatic.pro:<TRUE>:
# OTHER_FILES = "main.qml" "DefaultSceneEntity.qml"
# TEMPLATE = "app"
