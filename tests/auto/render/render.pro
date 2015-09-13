TEMPLATE = subdirs

contains(QT_CONFIG, private_tests) {
    SUBDIRS += \
        renderentity \
        renderqueue \
        renderrenderpass \
        qgraphicsutils \
        rendershader \
        renderviewutils \
        renderviews \
        rendermaterial \
        vsyncframeadvanceservice \
        meshfunctors \
        qmaterial \
        qattribute \
        qbuffer \
        qgeometry \
        qgeometryrenderer \
        buffer \
        attribute \
        rendergeometry \
        rendergeometryrenderer \
        raycasting
}
