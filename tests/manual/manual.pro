TEMPLATE = subdirs
qtHaveModule(quick) {
    SUBDIRS += \
        animations \
        basic \
        background \
        displaymodel \
        fullreferencing \
        model3ds \
        navigation1 \
        qrc \
        rotation \
        scaling \
        shapes \
        submesh \
        translation \
        transformations
}
