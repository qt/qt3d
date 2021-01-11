TEMPLATE = subdirs

qtConfig(private_tests) {
    SUBDIRS += \
        armature \
        aspect \
        attribute \
        blitframebuffer \
        buffer \
        computecommand \
        coordinatereader \
        ddstextures \
        effect \
        entity \
        filterentitybycomponent \
        filterkey \
        framegraphnode \
        framegraphvisitor \
        genericlambdajob \
        geometry \
        geometryrenderer \
        joint \
        ktxtextures \
        layerfiltering \
        levelofdetail \
        loadscenejob \
        material \
        memorybarrier \
        meshfunctors \
        objectpicker \
        parameter \
        proximityfilter \
        proximityfiltering \
        qabstractlight \
        qabstracttexture \
        qabstracttextureimage \
        qattribute \
        qblitframebuffer \
        qbuffer \
        qcamera \
        qcameralens \
        qcameraselector \
        qclearbuffers \
        qcomputecommand \
        qdispatchcompute \
        qeffect \
        qenvironmentlight \
        qfilterkey \
        qframegraphnode \
        qgeometry \
        qgeometryrenderer \
        qgraphicsapifilter \
        qlayerfilter \
        qlevelofdetail \
        qmemorybarrier \
        qmesh \
        qobjectpicker \
        qparameter \
        qproximityfilter \
        qraycaster \
        qrendercapture \
        qrenderpass \
        qrenderpassfilter \
        qrendersettings \
        qrenderstate \
        qrenderstateset \
        qrendersurfaceselector \
        qrendertarget \
        qrendertargetoutput \
        qrendertargetselector \
        qsceneloader \
        qscreenraycaster \
        qsetfence \
        qshaderimage \
        qshaderprogram \
        qshaderprogrambuilder \
        qsharedgltexture \
        qsortpolicy \
        qtechnique \
        qtechniquefilter \
        qtexturedataupdate \
        qtextureimage \
        qtextureimagedata \
        qtextureloader \
        qviewport \
        qwaitfence \
        raycaster \
        rendercapture \
        renderpass \
        rendertarget \
        sceneloader \
        segmentvisitor \
        setfence \
        shader \
        shaderbuilder \
        shadergraph \
        shaderimage \
        skeleton \
        sortpolicy \
        technique \
        texture \
        transform \
        trianglevisitor \
        uniform \
        vsyncframeadvanceservice \
        waitfence

    QT_FOR_CONFIG = 3dcore-private
    # TO DO: These could be restored to be executed in all cases
    # when aligned-malloc.pri becomes part of the test framework
    !qtConfig(qt3d-simd-avx2): {
      SUBDIRS += \
        qray3d \
        raycasting \
        triangleboundingvolume \
    }

    qtHaveModule(quick) {
      SUBDIRS += \
        raycastingjob
    }
}

# Tests related to the OpenGL renderer
QT_FOR_CONFIG += 3drender-private

qtConfig(qt3d-opengl-renderer):qtConfig(private_tests) {

    SUBDIRS += \
        opengl

    qtConfig(qt3d-extras) {
        qtHaveModule(quick) {
          SUBDIRS += \
            boundingsphere \
            pickboundingvolumejob \
            updateshaderdatatransformjob
        }

        SUBDIRS += \
            qmaterial \
            geometryloaders \
            picking \
            gltfplugins
    }

    qtConfig(qt3d-input) {
        qtHaveModule(quick) {
            SUBDIRS += \
                qscene2d \
                scene2d
        }
    }

    qtConfig(qt3d-simd-avx2): SUBDIRS += alignedresourcesmanagers-avx
    qtConfig(qt3d-simd-sse2):!qtConfig(qt3d-simd-avx2): SUBDIRS += alignedresourcesmanagers-sse
}

qtConfig(qt3d-rhi-renderer):qtConfig(private_tests):qtHaveModule(shadertools) {

    SUBDIRS += \
        rhi
}
