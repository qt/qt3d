TEMPLATE = subdirs

SUBDIRS += \
    qanimationaspect \
    qanimationcliploader \
    qclipanimator \
    qblendedclipanimator \
    qchannelmapping \
    qchannelmapper

qtConfig(private_tests) {
    SUBDIRS += \
        animationcliploader \
        qabstractclipblendnode \
        fcurve \
        functionrangefinder \
        bezierevaluator \
        clipanimator \
        blendedclipanimator \
        channelmapper \
        channelmapping \
        qlerpblend \
        clipblendnodemanager \
        clipblendnode \
        lerpblend \
        clipblendnodevisitor \
        qadditiveblend \
        additiveblend
}
