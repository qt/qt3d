// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QGEOMETRYRENDERER_P_H
#define QT3DRENDER_QGEOMETRYRENDERER_P_H

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

#include <Qt3DCore/private/qgeometryfactory_p.h>
#include <Qt3DCore/private/qboundingvolume_p.h>
#include <Qt3DRender/qgeometryrenderer.h>
#include <Qt3DRender/private/qt3drender_global_p.h>
#include <memory>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class Q_3DRENDERSHARED_PRIVATE_EXPORT QGeometryRendererPrivate : public Qt3DCore::QBoundingVolumePrivate
{
public:
    QGeometryRendererPrivate();
    ~QGeometryRendererPrivate();

    Q_DECLARE_PUBLIC(QGeometryRenderer)

    void setView(Qt3DCore::QGeometryView *view) override;

    int m_instanceCount;
    int m_vertexCount;
    int m_indexOffset;
    int m_firstInstance;
    int m_firstVertex;
    int m_indexBufferByteOffset;
    int m_restartIndexValue;
    int m_verticesPerPatch;
    bool m_primitiveRestart;
    Qt3DCore::QGeometry *m_geometry;
    QGeometryRenderer::PrimitiveType m_primitiveType;
    Qt3DCore::QGeometryFactoryPtr m_geometryFactory;
    float m_sortIndex;
};

} // namespace Qt3DRender

QT_END_NAMESPACE


#endif // QT3DRENDER_QGEOMETRYRENDERER_P_H

