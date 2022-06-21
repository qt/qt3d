// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qt3dquick3dextrasplugin.h"

#include <Qt3DExtras/qconegeometry.h>
#include <Qt3DExtras/qconegeometryview.h>
#include <Qt3DExtras/qconemesh.h>
#include <Qt3DExtras/qcuboidgeometry.h>
#include <Qt3DExtras/qcuboidgeometryview.h>
#include <Qt3DExtras/qcuboidmesh.h>
#include <Qt3DExtras/qcylindergeometry.h>
#include <Qt3DExtras/qcylindergeometryview.h>
#include <Qt3DExtras/qcylindermesh.h>
#include <Qt3DExtras/qdiffusemapmaterial.h>
#include <Qt3DExtras/qdiffusespecularmapmaterial.h>
#include <Qt3DExtras/qextrudedtextgeometry.h>
#include <Qt3DExtras/qextrudedtextmesh.h>
#include <Qt3DExtras/qfirstpersoncameracontroller.h>
#include <Qt3DExtras/qforwardrenderer.h>
#include <Qt3DExtras/qgoochmaterial.h>
#include <Qt3DExtras/qmetalroughmaterial.h>
#include <Qt3DExtras/qdiffusespecularmaterial.h>
#include <Qt3DExtras/qmorphphongmaterial.h>
#include <Qt3DExtras/qnormaldiffusemapalphamaterial.h>
#include <Qt3DExtras/qnormaldiffusemapmaterial.h>
#include <Qt3DExtras/qnormaldiffusespecularmapmaterial.h>
#include <Qt3DExtras/qorbitcameracontroller.h>
#include <Qt3DExtras/qpervertexcolormaterial.h>
#include <Qt3DExtras/qphongalphamaterial.h>
#include <Qt3DExtras/qphongmaterial.h>
#include <Qt3DExtras/qplanegeometry.h>
#include <Qt3DExtras/qplanegeometryview.h>
#include <Qt3DExtras/qplanemesh.h>
#include <Qt3DExtras/qskyboxentity.h>
#include <Qt3DExtras/qspheregeometry.h>
#include <Qt3DExtras/qspheregeometryview.h>
#include <Qt3DExtras/qspheremesh.h>
#include <Qt3DExtras/qspritegrid.h>
#include <Qt3DExtras/qspritesheetitem.h>
#include <Qt3DExtras/qtext2dentity.h>
#include <Qt3DExtras/qtexturematerial.h>
#include <Qt3DExtras/qtorusgeometry.h>
#include <Qt3DExtras/qtorusgeometryview.h>
#include <Qt3DExtras/qtorusmesh.h>

#include <Qt3DQuickExtras/private/quick3dlevelofdetailloader_p.h>
#include <Qt3DQuickExtras/private/quick3dspritesheet_p.h>

#include <QtQml/qqml.h>

QT_BEGIN_NAMESPACE

void Qt3DQuick3DExtrasPlugin::registerTypes(const char *uri)
{
    // Framegraphs
    qmlRegisterType<Qt3DExtras::QForwardRenderer>(uri, 2, 0, "ForwardRenderer");
    qmlRegisterType<Qt3DExtras::QForwardRenderer, 9>(uri, 2, 9, "ForwardRenderer");
    qmlRegisterType<Qt3DExtras::QForwardRenderer, 14>(uri, 2, 14, "ForwardRenderer");
    qmlRegisterType<Qt3DExtras::QForwardRenderer, 15>(uri, 2, 15, "ForwardRenderer");

    // Entities
    qmlRegisterType<Qt3DExtras::QSkyboxEntity>(uri, 2, 0, "SkyboxEntity");
    qmlRegisterType<Qt3DExtras::QSkyboxEntity, 9>(uri, 2, 9, "SkyboxEntity");
    qmlRegisterType<Qt3DExtras::Extras::Quick::Quick3DLevelOfDetailLoader>(uri, 2, 9, "LevelOfDetailLoader");

    // Camera Controllers
    qmlRegisterType<Qt3DExtras::QFirstPersonCameraController>(uri, 2, 0, "FirstPersonCameraController");
    qmlRegisterType<Qt3DExtras::QOrbitCameraController>(uri, 2, 0, "OrbitCameraController");

    // Materials
    qmlRegisterType<Qt3DExtras::QPhongMaterial>(uri, 2, 0, "PhongMaterial");
    qmlRegisterType<Qt3DExtras::QPhongAlphaMaterial>(uri, 2, 0, "PhongAlphaMaterial");
    qmlRegisterType<Qt3DExtras::QDiffuseMapMaterial>(uri, 2, 0, "DiffuseMapMaterial");
    qmlRegisterType<Qt3DExtras::QDiffuseSpecularMapMaterial>(uri, 2, 0, "DiffuseSpecularMapMaterial");
    qmlRegisterType<Qt3DExtras::QNormalDiffuseMapAlphaMaterial>(uri, 2, 0, "NormalDiffuseMapAlphaMaterial");
    qmlRegisterType<Qt3DExtras::QNormalDiffuseMapMaterial>(uri, 2, 0, "NormalDiffuseMapMaterial");
    qmlRegisterType<Qt3DExtras::QNormalDiffuseSpecularMapMaterial>(uri, 2, 0, "NormalDiffuseSpecularMapMaterial");
    qmlRegisterType<Qt3DExtras::QPerVertexColorMaterial>(uri, 2, 0, "PerVertexColorMaterial");
    qmlRegisterType<Qt3DExtras::QGoochMaterial>(uri, 2, 0, "GoochMaterial");
    qmlRegisterType<Qt3DExtras::QTextureMaterial>(uri, 2, 0, "TextureMaterial");
    qmlRegisterType<Qt3DExtras::QTextureMaterial, 10>(uri, 2, 10, "TextureMaterial");
    qmlRegisterType<Qt3DExtras::QTextureMaterial, 11>(uri, 2, 11, "TextureMaterial");
    qmlRegisterType<Qt3DExtras::QDiffuseSpecularMaterial>(uri, 2, 10, "DiffuseSpecularMaterial");
    qmlRegisterType<Qt3DExtras::QMetalRoughMaterial>(uri, 2, 9, "MetalRoughMaterial");
    qmlRegisterType<Qt3DExtras::QMetalRoughMaterial, 10>(uri, 2, 10, "MetalRoughMaterial");

    qmlRegisterType<Qt3DExtras::QSpriteGrid>(uri, 2, 10, "SpriteGrid");
    qmlRegisterType<Qt3DExtras::QSpriteSheetItem>(uri, 2, 10, "SpriteItem");
    Qt3DExtras::Quick::registerExtendedType<Qt3DExtras::QSpriteSheet, Qt3DExtras::Extras::Quick::Quick3DSpriteSheet>("QSpriteSheet", "Qt3D.Extras/SpriteSheet", uri, 2, 10, "SpriteSheet");

    // Meshes
    qmlRegisterType<Qt3DExtras::QConeMesh>(uri, 2, 0, "ConeMesh");
    qmlRegisterType<Qt3DExtras::QConeGeometry>(uri, 2, 0, "ConeGeometry");
    qmlRegisterType<Qt3DExtras::QConeGeometryView>(uri, 2, 16, "ConeGeometryView");
    qmlRegisterType<Qt3DExtras::QCuboidMesh>(uri, 2, 0, "CuboidMesh");
    qmlRegisterType<Qt3DExtras::QCuboidGeometry>(uri, 2, 0, "CuboidGeometry");
    qmlRegisterType<Qt3DExtras::QCuboidGeometryView>(uri, 2, 16, "CuboidGeometryView");
    qmlRegisterType<Qt3DExtras::QCylinderMesh>(uri, 2, 0, "CylinderMesh");
    qmlRegisterType<Qt3DExtras::QCylinderGeometry>(uri, 2, 0, "CylinderGeometry");
    qmlRegisterType<Qt3DExtras::QCylinderGeometryView>(uri, 2, 16, "CylinderGeometryView");
    qmlRegisterType<Qt3DExtras::QPlaneMesh>(uri, 2, 0, "PlaneMesh");
    qmlRegisterType<Qt3DExtras::QPlaneMesh, 9>(uri, 2, 9, "PlaneMesh");
    qmlRegisterType<Qt3DExtras::QPlaneGeometry>(uri, 2, 0, "PlaneGeometry");
    qmlRegisterType<Qt3DExtras::QPlaneGeometryView>(uri, 2, 16, "PlaneGeometryView");
    qmlRegisterType<Qt3DExtras::QPlaneGeometry, 9>(uri, 2, 9, "PlaneGeometry");
    qmlRegisterType<Qt3DExtras::QTorusMesh>(uri, 2, 0, "TorusMesh");
    qmlRegisterType<Qt3DExtras::QTorusGeometry>(uri, 2, 0, "TorusGeometry");
    qmlRegisterType<Qt3DExtras::QTorusGeometryView>(uri, 2, 16, "TorusGeometryView");
    qmlRegisterType<Qt3DExtras::QSphereMesh>(uri, 2, 0, "SphereMesh");
    qmlRegisterType<Qt3DExtras::QSphereGeometry>(uri, 2, 0, "SphereGeometry");
    qmlRegisterType<Qt3DExtras::QSphereGeometryView>(uri, 2, 16, "SphereGeometryView");

    // 3D Text
    qmlRegisterType<Qt3DExtras::QExtrudedTextGeometry>(uri, 2, 9, "ExtrudedTextGeometry");
    qmlRegisterType<Qt3DExtras::QExtrudedTextMesh>(uri, 2, 9, "ExtrudedTextMesh");

    qmlRegisterType<Qt3DExtras::QText2DEntity>(uri, 2, 9, "Text2DEntity");

    // Auto-increment the import to stay in sync with ALL future Qt minor versions
    qmlRegisterModule(uri, 2, 15);
}


QT_END_NAMESPACE

#include "moc_qt3dquick3dextrasplugin.cpp"
