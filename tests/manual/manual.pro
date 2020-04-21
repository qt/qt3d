TEMPLATE = subdirs

SUBDIRS += \
    bigscene-cpp \
    component-changes \
    custom-mesh-cpp \
    custom-mesh-cpp-indirect \
    custom-mesh-update-data-cpp \
    cylinder-cpp \
    cylinder-parent-test \
    deferred-renderer-cpp \
    raster-cpp \
    qtbug-72236 \
    manual-renderloop \
    rhi


qtHaveModule(multimedia): {
    SUBDIRS += \
        sharedtexture

    qtHaveModule(quick) {
        SUBDIRS += \
            sharedtextureqml
    }
}

qtHaveModule(widgets): {
    SUBDIRS += \
        assimp-cpp \
        paintedtexture-cpp \
        rendercapture-cpp \
        texture-updates-cpp
}

qtHaveModule(quick) {
    !macos:!uikit: SUBDIRS += compute-manual

    SUBDIRS += \
        assimp \
        animation-keyframe-simple \
        animation-keyframe-blendtree \
        animation-keyframe-programmatic \
        bigmodel-qml \
        bigscene-instanced-qml \
        clip-planes-qml \
        custom-mesh-qml \
        custom-mesh-update-data-qml \
        cylinder-qml \
        deferred-renderer-qml \
        downloading \
        dynamicscene-cpp \
        dragging \
        enabled-qml \
        gltf \
        gooch-qml \
        keyboardinput-qml \
        lod \
        loader-qml \
        mouseinput-qml \
        multiplewindows-qml \
        plasma \
        pointlinesize \
        scene3d-loader \
        picking-qml \
        skybox \
        simple-shaders-qml \
        transparency-qml \
        transparency-qml-scene3d \
        rendercapture-qml \
        additional-attributes-qml \
        dynamic-model-loader-qml \
        buffercapture-qml \
        render-qml-to-texture \
        render-qml-to-texture-qml \
        video-texture-qml \
        transforms-qml \
        layerfilter-qml \
        tessellation-modes \
        rendercapture-qml-fbo \
        blitframebuffer-qml \
        raycasting-qml \
        raster-qml \
        shader-image-qml \
        spritegrid \
        subtree-enabler-qml \
        distancefieldtext \
        mesh-morphing \
        anim-viewer \
        skinned-mesh \
        rigged-simple \
        proximityfilter \
        scene3d-visibility \
        shared_texture_image \
        texture_property_updates \
        qtbug-76766 \
        scene3d-in-sync \
        compressed_textures \
}

qtHaveModule(quickwidgets): SUBDIRS += quickwidget-switch
