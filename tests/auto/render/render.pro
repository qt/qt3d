TEMPLATE = subdirs

contains(QT_CONFIG, private_tests) {
    SUBDIRS += \
        renderentity \
        renderqueues \
        renderrenderpass \
        qgraphicsutils \
        rendershader \
        renderviewutils \
        renderviews \
        rendermaterial \
        rendermesh
}
