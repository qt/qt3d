TEMPLATE = subdirs

src_core.subdir = $$PWD/core
src_core.target = sub-core

src_render.subdir = $$PWD/render
src_render.target = sub-render
src_render.depends = src_core

src_bulletphysics.subdir = $$PWD/bulletphysics
src_bulletphysics.target = sub-bulletphysics
src_bulletphysics.depends = src_core

src_openal.subdir = $$PWD/openal
src_openal.target = sub-openal
src_openal.depends = src_core


# Quick3D libs
src_quick3d_core.subdir = $$PWD/quick3d/quick3d
src_quick3d_core.target = sub-quick3d-core
src_quick3d_core.depends = src_core

src_quick3d_render.subdir = $$PWD/quick3d/quick3drenderer
src_quick3d_render.target = sub-quick3d-render
src_quick3d_render.depends = src_render


# Quick3D imports
src_quick3d_core_imports.subdir = $$PWD/quick3d/imports/core
src_quick3d_core_imports.target = sub-quick3d-imports-core
src_quick3d_core_imports.depends = src_quick3d_core

src_quick3d_imports_render.subdir = $$PWD/quick3d/imports/render
src_quick3d_imports_render.target = sub-quick3d-imports-render
src_quick3d_imports_render.depends = src_quick3d_render

src_quick3d_imports_bulletphysics.subdir = $$PWD/quick3d/imports/bulletphysics
src_quick3d_imports_bulletphysics.target = sub-quick3d-imports-bulletphysics
src_quick3d_imports_bulletphysics.depends = src_bulletphysics


SUBDIRS += \
    src_core \
    src_render \
    src_openal \
    src_quick3d_core \
    src_quick3d_core_imports \
    src_quick3d_render \
    src_quick3d_imports_render

config_bullet {
    SUBDIRS += \
        src_bulletphysics \
        src_quick3d_imports_bulletphysics
}


