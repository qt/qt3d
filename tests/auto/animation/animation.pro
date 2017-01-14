TEMPLATE = subdirs

SUBDIRS += \
    qanimationaspect \
    qanimationclip

qtConfig(private_tests) {
    SUBDIRS += \
        animationclip
}
