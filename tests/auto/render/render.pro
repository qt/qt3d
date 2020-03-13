TEMPLATE = subdirs

qtConfig(private_tests) {
    SUBDIRS += \
        aspect \
        entity \
        renderpass \
        shader \
        shaderbuilder \
        texture \
        material \
        vsyncframeadvanceservice \
        meshfunctors \
        qattribute \
        qbuffer \
        qgeometry \
        qgeometryrenderer \
        qlevelofdetail \
        levelofdetail \
        buffer \
        attribute \
        geometry \
        geometryrenderer \
        qcameraselector \
        qclearbuffers \
        qframegraphnode \
        qlayerfilter \
        qabstractlight \
        qenvironmentlight \
        qrenderpassfilter \
        qrenderstate \
        qrendertargetselector \
        qsortpolicy \
        qrenderstateset \
        qtechniquefilter \
        qtextureimagedata \
        qviewport \
        framegraphnode \
        qobjectpicker \
        objectpicker \
#        qboundingvolumedebug \
#        boundingvolumedebug \
        ddstextures \
        ktxtextures \
        layerfiltering \
        filterentitybycomponent \
        genericlambdajob \
        qgraphicsapifilter \
        qrendersurfaceselector \
        sortpolicy \
        sceneloader \
        qsceneloader \
        qrendertargetoutput \
        qcameralens \
        qcomputecommand \
        loadscenejob \
        qrendercapture \
        uniform \
        qparameter \
        parameter \
        qtextureloader \
        qsharedgltexture \
        qtextureimage \
        qabstracttexture \
        qabstracttextureimage \
        qrendersettings \
        rendertarget \
        transform \
        computecommand \
        qrendertarget \
        qdispatchcompute \
        qtechnique \
        qeffect \
        qrenderpass \
        qfilterkey \
        effect \
        filterkey \
        qmesh \
        technique \
        rendercapture \
        segmentvisitor \
        trianglevisitor \
        qmemorybarrier \
        memorybarrier \
        qshaderprogram \
        qshaderprogrambuilder \
        coordinatereader \
        framegraphvisitor \
        armature \
        skeleton \
        joint \
        qproximityfilter \
        proximityfilter \
        proximityfiltering \
        qblitframebuffer \
        blitframebuffer \
        qraycaster \
        raycaster \
        qscreenraycaster \
        qcamera \
        qsetfence \
        qwaitfence \
        setfence \
        waitfence \
        qtexturedataupdate \
        qshaderimage \
        shaderimage \
        shadergraph

    QT_FOR_CONFIG = 3dcore-private
    # TO DO: These could be restored to be executed in all cases
    # when aligned-malloc.pri becomes part of the test framework
    !qtConfig(qt3d-simd-avx2): {
      SUBDIRS += \
        qray3d \
        raycasting \
        trianglesextractor \
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
            updatemeshtrianglelistjob \
            updateshaderdatatransformjob
        }

        SUBDIRS += \
            qmaterial \
            geometryloaders \
            picking \
            qdefaultmeshes \
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
