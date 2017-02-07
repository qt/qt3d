TEMPLATE = subdirs

qtConfig(private_tests) {
    SUBDIRS += \
        qcollider \
        collider \
        qphysicsworld \
        physicsworld \
        qphysicsmaterial \
        physicsmaterial \
        qcuboidcollisionshape
}
