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
        qclearbuffer \
        qframegraphnode \
        qframegraph \
        qlayerfilter \
        qlight \
        qrenderpassfilter \
        qrendertargetselector \
        qsortcriterion \
        qsortmethod \
        qstateset \
        qtechniquefilter \
        qviewport \
        framegraphnode \
        qobjectpicker \
        objectpicker \
        picking \
        qboundingvolumedebug \
        boundingvolumedebug \
        qdefaultmeshes \
        trianglesextractor \
        triangleboundingvolume
}
