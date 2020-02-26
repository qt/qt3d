TEMPLATE = subdirs

SUBDIRS += \
        filtercompatibletechniquejob \
        graphicshelpergl3_3 \
        graphicshelpergl3_2 \
        graphicshelpergl2 \
        glshadermanager \
        textures \
        renderer \
        renderviewutils \
        renderviews \
        renderqueue \
        renderviewbuilder \
        qgraphicsutils \
        computecommand

qtHaveModule(quick) {
    SUBDIRS += \
        materialparametergathererjob
}
!macos: SUBDIRS += graphicshelpergl4
