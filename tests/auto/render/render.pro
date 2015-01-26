TEMPLATE = subdirs

contains(QT_CONFIG, private_tests) {
    SUBDIRS += \
        renderentity \
        renderqueues \
        qgraphicsutils
}
