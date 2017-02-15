TEMPLATE = subdirs

SUBDIRS += \
    qanimationaspect \
    qanimationclip \
    qclipanimator \
    qblendedclipanimator \
    qchannelmapping \
    qchannelmapper

qtConfig(private_tests) {
    SUBDIRS += \
        animationclip \
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
