TEMPLATE = subdirs

SUBDIRS += \
    qanimationaspect \
    qanimationclip \
    qclipanimator \
    qblendedclipanimator \
    qconductedclipanimator

qtConfig(private_tests) {
    SUBDIRS += \
        animationclip \
        qabstractclipblendnode \
        fcurve \
        functionrangefinder \
        bezierevaluator \
        clipanimator \
        blendedclipanimator \
        conductedclipanimator
}
