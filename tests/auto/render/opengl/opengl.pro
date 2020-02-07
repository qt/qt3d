TEMPLATE = subdirs

SUBDIRS += \
        filtercompatibletechniquejob \
        graphicshelpergl3_3 \
        graphicshelpergl3_2 \
        graphicshelpergl2 \
        glshadermanager \
        materialparametergathererjob \
        textures \
        renderer \
        renderviewutils \
        renderviews \
        renderqueue \
        renderviewbuilder \
        qgraphicsutils \
        computecommand

!macos: SUBDIRS += graphicshelpergl4
