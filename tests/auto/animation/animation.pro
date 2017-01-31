TEMPLATE = subdirs

SUBDIRS += \
    qanimationaspect \
    qanimationclip \
    qclipanimator \
    qblendedclipanimator \
    qconductedclipanimator \
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
        conductedclipanimator \
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
