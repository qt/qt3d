TEMPLATE = subdirs

contains(QT_CONFIG, private_tests) {
    SUBDIRS += \
        entity \
        renderqueue \
        renderpass \
        qgraphicsutils \
        shader \
        renderviewutils \
        renderviews \
        material \
        vsyncframeadvanceservice \
        meshfunctors \
        qmaterial \
        qattribute \
        qbuffer \
        qgeometry \
        qgeometryrenderer \
        buffer \
        attribute \
        geometry \
        geometryrenderer \
        raycasting \
        qcameraselector \
        qclearbuffers \
        qframegraphnode \
        qlayerfilter \
        qabstractlight \
        qray3d \
        qrenderpassfilter \
        qrendertargetselector \
        qsortpolicy \
        qrenderstateset \
        qtechniquefilter \
        qtextureimagedata \
        qviewport \
        framegraphnode \
        qobjectpicker \
        objectpicker \
        picking \
#        qboundingvolumedebug \
#        boundingvolumedebug \
        qdefaultmeshes \
        trianglesextractor \
        triangleboundingvolume \
        ddstextures \
        shadercache
}
