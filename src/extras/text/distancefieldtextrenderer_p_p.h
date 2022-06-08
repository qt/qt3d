// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DEXTRAS_DISTANCEFIELDTEXTRENDERER_P_P_H
#define QT3DEXTRAS_DISTANCEFIELDTEXTRENDERER_P_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <Qt3DCore/private/qentity_p.h>
#include <Qt3DExtras/private/distancefieldtextrenderer_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {
class QGeometry;
class QMaterial;
class QAttribute;
class QBuffer;
}

namespace Qt3DRender {
class QGeometryRenderer;
}

namespace Qt3DExtras {

class QText2DMaterial;

class DistanceFieldTextRendererPrivate : public Qt3DCore::QEntityPrivate
{
public:
    DistanceFieldTextRendererPrivate();
    ~DistanceFieldTextRendererPrivate();

    Q_DECLARE_PUBLIC(DistanceFieldTextRenderer)

    void init();

    Qt3DRender::QGeometryRenderer *m_renderer;
    Qt3DCore::QGeometry *m_geometry;
    Qt3DCore::QAttribute *m_positionAttr;
    Qt3DCore::QAttribute *m_texCoordAttr;
    Qt3DCore::QAttribute *m_indexAttr;
    Qt3DCore::QBuffer *m_vertexBuffer;
    Qt3DCore::QBuffer *m_indexBuffer;
    QText2DMaterial *m_material;
};

} // namespace Qt3DExtras

QT_END_NAMESPACE

#endif // QT3DEXTRAS_DISTANCEFIELDTEXTRENDERER_P_P_H
