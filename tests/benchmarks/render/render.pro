TEMPLATE=subdirs

qtConfig(private_tests) {
    SUBDIRS += layerfiltering \
               materialparametergathering \
               opengl

    qtHaveModule(quick): \
        SUBDIRS += jobs
}
