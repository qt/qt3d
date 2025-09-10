// Copyright (C) 2024 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DQUICK3DEXTRASFOREIGN_P_H
#define QT3DQUICK3DEXTRASFOREIGN_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtQml/qqml.h>

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
#include <Qt3DExtras/qtext2dentity.h>
#include <Qt3DExtras/qtexturematerial.h>
#include <Qt3DExtras/qtorusgeometry.h>
#include <Qt3DExtras/qtorusgeometryview.h>
#include <Qt3DExtras/qtorusmesh.h>

#include <Qt3DQuickExtras/private/quick3dspritesheet_p.h>

QT_BEGIN_NAMESPACE

class QConeGeometryForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(ConeGeometry)
    QML_FOREIGN(Qt3DExtras::QConeGeometry)
    QML_ADDED_IN_VERSION(2, 0)
};

class QConeGeometryViewForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(ConeGeometryView)
    QML_FOREIGN(Qt3DExtras::QConeGeometryView)
    QML_ADDED_IN_VERSION(2, 16)
};

class QConeMeshForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(ConeMesh)
    QML_FOREIGN(Qt3DExtras::QConeMesh)
    QML_ADDED_IN_VERSION(2, 0)
};

class QCuboidGeometryForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(CuboidGeometry)
    QML_FOREIGN(Qt3DExtras::QCuboidGeometry)
    QML_ADDED_IN_VERSION(2, 0)
};

class QCuboidGeometryViewForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(CuboidGeometryView)
    QML_FOREIGN(Qt3DExtras::QCuboidGeometryView)
    QML_ADDED_IN_VERSION(2, 16)
};

class QCuboidMeshForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(CuboidMesh)
    QML_FOREIGN(Qt3DExtras::QCuboidMesh)
    QML_ADDED_IN_VERSION(2, 0)
};

class QCylinderGeometryForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(CylinderGeometry)
    QML_FOREIGN(Qt3DExtras::QCylinderGeometry)
    QML_ADDED_IN_VERSION(2, 0)
};

class QCylinderGeometryViewForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(CylinderGeometryView)
    QML_FOREIGN(Qt3DExtras::QCylinderGeometryView)
    QML_ADDED_IN_VERSION(2, 16)
};

class QCylinderMeshForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(CylinderMesh)
    QML_FOREIGN(Qt3DExtras::QCylinderMesh)
    QML_ADDED_IN_VERSION(2, 0)
};

class QDiffuseMapMaterialForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(DiffuseMapMaterial)
    QML_FOREIGN(Qt3DExtras::QDiffuseMapMaterial)
    QML_ADDED_IN_VERSION(2, 0)
};

class QDiffuseSpecularMapMaterialForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(DiffuseSpecularMapMaterial)
    QML_FOREIGN(Qt3DExtras::QDiffuseSpecularMapMaterial)
    QML_ADDED_IN_VERSION(2, 0)
};

class QDiffuseSpecularMaterialForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(DiffuseSpecularMaterial)
    QML_FOREIGN(Qt3DExtras::QDiffuseSpecularMaterial)
    QML_ADDED_IN_VERSION(2, 10)
};

class QExtrudedTextGeometryForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(ExtrudedTextGeometry)
    QML_FOREIGN(Qt3DExtras::QExtrudedTextGeometry)
    QML_ADDED_IN_VERSION(2, 9)
};

class QExtrudedTextMeshForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(ExtrudedTextMesh)
    QML_FOREIGN(Qt3DExtras::QExtrudedTextMesh)
    QML_ADDED_IN_VERSION(2, 9)
};

class QFirstPersonCameraControllerForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(FirstPersonCameraController)
    QML_FOREIGN(Qt3DExtras::QFirstPersonCameraController)
    QML_ADDED_IN_VERSION(2, 0)
};

class QForwardRendererForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(ForwardRenderer)
    QML_FOREIGN(Qt3DExtras::QForwardRenderer)
    QML_ADDED_IN_VERSION(2, 0)
};

class QGoochMaterialForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(GoochMaterial)
    QML_FOREIGN(Qt3DExtras::QGoochMaterial)
    QML_ADDED_IN_VERSION(2, 0)
};

class QMetalRoughMaterialForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(MetalRoughMaterial)
    QML_FOREIGN(Qt3DExtras::QMetalRoughMaterial)
    QML_ADDED_IN_VERSION(2, 9)
};

class QNormalDiffuseMapAlphaMaterialForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(NormalDiffuseMapAlphaMaterial)
    QML_FOREIGN(Qt3DExtras::QNormalDiffuseMapAlphaMaterial)
    QML_ADDED_IN_VERSION(2, 0)
};

class QNormalDiffuseMapMaterialForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(NormalDiffuseMapMaterial)
    QML_FOREIGN(Qt3DExtras::QNormalDiffuseMapMaterial)
    QML_ADDED_IN_VERSION(2, 0)
};

class QNormalDiffuseSpecularMapMaterialForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(NormalDiffuseSpecularMapMaterial)
    QML_FOREIGN(Qt3DExtras::QNormalDiffuseSpecularMapMaterial)
    QML_ADDED_IN_VERSION(2, 0)
};

class QOrbitCameraControllerForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(OrbitCameraController)
    QML_FOREIGN(Qt3DExtras::QOrbitCameraController)
    QML_ADDED_IN_VERSION(2, 0)
};

class QPerVertexColorMaterialForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(PerVertexColorMaterial)
    QML_FOREIGN(Qt3DExtras::QPerVertexColorMaterial)
    QML_ADDED_IN_VERSION(2, 0)
};

class QPhongAlphaMaterialForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(PhongAlphaMaterial)
    QML_FOREIGN(Qt3DExtras::QPhongAlphaMaterial)
    QML_ADDED_IN_VERSION(2, 0)
};

class QPhongMaterialForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(PhongMaterial)
    QML_FOREIGN(Qt3DExtras::QPhongMaterial)
    QML_ADDED_IN_VERSION(2, 0)
};

class QPlaneGeometryForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(PlaneGeometry)
    QML_FOREIGN(Qt3DExtras::QPlaneGeometry)
    QML_ADDED_IN_VERSION(2, 0)
};

class QPlaneGeometryViewForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(PlaneGeometryView)
    QML_FOREIGN(Qt3DExtras::QPlaneGeometryView)
    QML_ADDED_IN_VERSION(2, 16)
};

class QPlaneMeshForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(PlaneMesh)
    QML_FOREIGN(Qt3DExtras::QPlaneMesh)
    QML_ADDED_IN_VERSION(2, 0)
};

class QSkyboxEntityForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(SkyboxEntity)
    QML_FOREIGN(Qt3DExtras::QSkyboxEntity)
    QML_ADDED_IN_VERSION(2, 0)
};

class QSphereGeometryForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(SphereGeometry)
    QML_FOREIGN(Qt3DExtras::QSphereGeometry)
    QML_ADDED_IN_VERSION(2, 0)
};

class QSphereGeometryViewForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(SphereGeometryView)
    QML_FOREIGN(Qt3DExtras::QSphereGeometryView)
    QML_ADDED_IN_VERSION(2, 16)
};

class QSphereMeshForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(SphereMesh)
    QML_FOREIGN(Qt3DExtras::QSphereMesh)
    QML_ADDED_IN_VERSION(2, 0)
};

class QSpriteGridForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(SpriteGrid)
    QML_FOREIGN(Qt3DExtras::QSpriteGrid)
    QML_ADDED_IN_VERSION(2, 10)
};

class QSpriteItemForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(SpriteItem)
    QML_FOREIGN(Qt3DExtras::QSpriteSheetItem)
    QML_ADDED_IN_VERSION(2, 10)
};

class QText2DEntityForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(Text2DEntity)
    QML_FOREIGN(Qt3DExtras::QText2DEntity)
    QML_ADDED_IN_VERSION(2, 9)
};

class QTextureMaterialForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(TextureMaterial)
    QML_FOREIGN(Qt3DExtras::QTextureMaterial)
    QML_ADDED_IN_VERSION(2, 0)
};

class QTorusGeometryForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(TorusGeometry)
    QML_FOREIGN(Qt3DExtras::QTorusGeometry)
    QML_ADDED_IN_VERSION(2, 0)
};

class QTorusGeometryViewForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(TorusGeometryView)
    QML_FOREIGN(Qt3DExtras::QTorusGeometryView)
    QML_ADDED_IN_VERSION(2, 16)
};

class QTorusMeshForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(TorusMesh)
    QML_FOREIGN(Qt3DExtras::QTorusMesh)
    QML_ADDED_IN_VERSION(2, 0)
};

QT_END_NAMESPACE

#endif // QT3DQUICK3DEXTRASFOREIGN_P_H
