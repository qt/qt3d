# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from extras.pro.

#####################################################################
## Qt::3DExtras Module:
#####################################################################

qt_internal_add_module(3DExtras
    SOURCES
        3dtext/qextrudedtextgeometry.cpp 3dtext/qextrudedtextgeometry.h 3dtext/qextrudedtextgeometry_p.h
        3dtext/qextrudedtextmesh.cpp 3dtext/qextrudedtextmesh.h
        defaults/qabstractcameracontroller.cpp defaults/qabstractcameracontroller.h defaults/qabstractcameracontroller_p.h
        defaults/qabstractspritesheet.cpp defaults/qabstractspritesheet.h defaults/qabstractspritesheet_p.h
        defaults/qdiffusemapmaterial.cpp defaults/qdiffusemapmaterial.h defaults/qdiffusemapmaterial_p.h
        defaults/qdiffusespecularmapmaterial.cpp defaults/qdiffusespecularmapmaterial.h defaults/qdiffusespecularmapmaterial_p.h
        defaults/qdiffusespecularmaterial.cpp defaults/qdiffusespecularmaterial.h defaults/qdiffusespecularmaterial_p.h
        defaults/qfirstpersoncameracontroller.cpp defaults/qfirstpersoncameracontroller.h
        defaults/qforwardrenderer.cpp defaults/qforwardrenderer.h defaults/qforwardrenderer_p.h
        defaults/qgoochmaterial.cpp defaults/qgoochmaterial.h defaults/qgoochmaterial_p.h
        defaults/qmetalroughmaterial.cpp defaults/qmetalroughmaterial.h defaults/qmetalroughmaterial_p.h
        defaults/qmorphphongmaterial.cpp defaults/qmorphphongmaterial.h defaults/qmorphphongmaterial_p.h
        defaults/qnormaldiffusemapalphamaterial.cpp defaults/qnormaldiffusemapalphamaterial.h defaults/qnormaldiffusemapalphamaterial_p.h
        defaults/qnormaldiffusemapmaterial.cpp defaults/qnormaldiffusemapmaterial.h defaults/qnormaldiffusemapmaterial_p.h
        defaults/qnormaldiffusespecularmapmaterial.cpp defaults/qnormaldiffusespecularmapmaterial.h defaults/qnormaldiffusespecularmapmaterial_p.h
        defaults/qorbitcameracontroller.cpp defaults/qorbitcameracontroller.h defaults/qorbitcameracontroller_p.h
        defaults/qpervertexcolormaterial.cpp defaults/qpervertexcolormaterial.h defaults/qpervertexcolormaterial_p.h
        defaults/qphongalphamaterial.cpp defaults/qphongalphamaterial.h defaults/qphongalphamaterial_p.h
        defaults/qphongmaterial.cpp defaults/qphongmaterial.h defaults/qphongmaterial_p.h
        defaults/qskyboxentity.cpp defaults/qskyboxentity.h defaults/qskyboxentity_p.h
        defaults/qspritegrid.cpp defaults/qspritegrid.h defaults/qspritegrid_p.h
        defaults/qspritesheet.cpp defaults/qspritesheet.h defaults/qspritesheet_p.h
        defaults/qspritesheetitem.cpp defaults/qspritesheetitem.h defaults/qspritesheetitem_p.h
        defaults/qt3dwindow.cpp defaults/qt3dwindow.h defaults/qt3dwindow_p.h
        defaults/qtexturematerial.cpp defaults/qtexturematerial.h defaults/qtexturematerial_p.h
        geometries/qconegeometry.cpp geometries/qconegeometry.h geometries/qconegeometry_p.h
        geometries/qconegeometryview.cpp geometries/qconegeometryview.h
        geometries/qconemesh.cpp geometries/qconemesh.h
        geometries/qcuboidgeometry.cpp geometries/qcuboidgeometry.h geometries/qcuboidgeometry_p.h
        geometries/qcuboidgeometryview.cpp geometries/qcuboidgeometryview.h
        geometries/qcuboidmesh.cpp geometries/qcuboidmesh.h
        geometries/qcylindergeometry.cpp geometries/qcylindergeometry.h geometries/qcylindergeometry_p.h
        geometries/qcylindergeometryview.cpp geometries/qcylindergeometryview.h
        geometries/qcylindermesh.cpp geometries/qcylindermesh.h
        geometries/qplanegeometry.cpp geometries/qplanegeometry.h geometries/qplanegeometry_p.h
        geometries/qplanegeometryview.cpp geometries/qplanegeometryview.h
        geometries/qplanemesh.cpp geometries/qplanemesh.h
        geometries/qspheregeometry.cpp geometries/qspheregeometry.h geometries/qspheregeometry_p.h
        geometries/qspheregeometryview.cpp geometries/qspheregeometryview.h
        geometries/qspheremesh.cpp geometries/qspheremesh.h
        geometries/qtorusgeometry.cpp geometries/qtorusgeometry.h geometries/qtorusgeometry_p.h
        geometries/qtorusgeometryview.cpp geometries/qtorusgeometryview.h
        geometries/qtorusmesh.cpp geometries/qtorusmesh.h
        qt3dextras_global.h
        text/areaallocator.cpp text/areaallocator_p.h
        text/distancefieldtextrenderer.cpp text/distancefieldtextrenderer_p.h
        text/distancefieldtextrenderer_p_p.h
        text/qdistancefieldglyphcache.cpp text/qdistancefieldglyphcache_p.h
        text/qtext2dentity.cpp text/qtext2dentity.h text/qtext2dentity_p.h
        text/qtext2dmaterial.cpp text/qtext2dmaterial_p.h
        text/qtext2dmaterial_p_p.h
        text/qtextureatlas.cpp text/qtextureatlas_p.h
        text/qtextureatlas_p_p.h
    DEFINES
        QT3DEXTRAS_LIBRARY
    INCLUDE_DIRECTORIES
        3dtext
        defaults
        geometries
        text
    LIBRARIES
        Qt::3DCorePrivate
        Qt::3DRenderPrivate
        Qt::CorePrivate
    PUBLIC_LIBRARIES
        Qt::Core
        Qt::Gui
        Qt::3DCore
        Qt::3DInput
        Qt::3DLogic
        Qt::3DRender
    PRIVATE_MODULE_INTERFACE
        Qt::3DCorePrivate
        Qt::3DRenderPrivate
        Qt::CorePrivate
)

# QTransform conflicts
set_target_properties(3DExtras PROPERTIES UNITY_BUILD OFF)

# Resources:
set(extras_resource_files
    "shaders/es2/coordinatesystems.inc"
    "shaders/es2/default.vert"
    "shaders/es2/distancefieldtext.frag"
    "shaders/es2/distancefieldtext.vert"
    "shaders/es2/gooch.frag"
    "shaders/es2/gooch.vert"
    "shaders/es2/light.inc.frag"
    "shaders/es2/light.inc.frag100"
    "shaders/es2/morphphong.vert"
    "shaders/es2/pervertexcolor.frag"
    "shaders/es2/pervertexcolor.vert"
    "shaders/es2/phong.inc.frag"
    "shaders/es2/phong.inc.frag100"
    "shaders/es2/skybox.frag"
    "shaders/es2/skybox.vert"
    "shaders/es2/unlittexture.frag"
    "shaders/es2/unlittexture.vert"
    "shaders/es3/coordinatesystems.inc"
    "shaders/es3/default.vert"
    "shaders/es3/light.inc.frag"
    "shaders/es3/metalrough.inc.frag"
    "shaders/gl3/coordinatesystems.inc"
    "shaders/gl3/default.vert"
    "shaders/gl3/distancefieldtext.frag"
    "shaders/gl3/distancefieldtext.vert"
    "shaders/gl3/gooch.frag"
    "shaders/gl3/gooch.vert"
    "shaders/gl3/light.inc.frag"
    "shaders/gl3/metalrough.inc.frag"
    "shaders/gl3/morphphong.vert"
    "shaders/gl3/pervertexcolor.frag"
    "shaders/gl3/pervertexcolor.vert"
    "shaders/gl3/phong.inc.frag"
    "shaders/gl3/skybox.frag"
    "shaders/gl3/skybox.vert"
    "shaders/gl3/unlittexture.frag"
    "shaders/gl3/unlittexture.vert"
    "shaders/graphs/metalrough.frag.json"
    "shaders/graphs/phong.frag.json"
    "shaders/rhi/coordinatesystems.inc"
    "shaders/rhi/default_pos_norm.vert"
    "shaders/rhi/default_pos_norm_tex.vert"
    "shaders/rhi/default_pos_norm_tex_tan.vert"
    "shaders/rhi/default_pos_tex.vert"
    "shaders/rhi/defaultuniforms.inc"
    "shaders/rhi/distancefieldtext.frag"
    "shaders/rhi/distancefieldtext.vert"
    "shaders/rhi/gooch.frag"
    "shaders/rhi/gooch.vert"
    "shaders/rhi/light.inc.frag"
    "shaders/rhi/metalrough.inc.frag"
    "shaders/rhi/morphphong.vert"
    "shaders/rhi/pervertexcolor.frag"
    "shaders/rhi/pervertexcolor.vert"
    "shaders/rhi/phong.inc.frag"
    "shaders/rhi/skybox.frag"
    "shaders/rhi/skybox.vert"
    "shaders/rhi/unlittexture.frag"
    "shaders/rhi/unlittexture.vert"
)

qt_internal_add_resource(3DExtras "extras"
    PREFIX
        "/"
    FILES
        ${extras_resource_files}
)


#### Keys ignored in scope 1:.:.:extras.pro:<TRUE>:
# MODULE = "3DExtras"
