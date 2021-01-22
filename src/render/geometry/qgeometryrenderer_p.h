/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
****************************************************************************/

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

#include <Qt3DCore/private/qcomponent_p.h>
#include <Qt3DRender/qgeometryrenderer.h>
#include <Qt3DRender/qgeometryfactory.h>
#include <Qt3DRender/private/qt3drender_global_p.h>
#include <Qt3DCore/private/qtypedpropertyupdatechange_p.h>
#include <memory>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class Q_3DRENDERSHARED_PRIVATE_EXPORT QGeometryRendererPrivate : public Qt3DCore::QComponentPrivate
{
public:
    QGeometryRendererPrivate();
    ~QGeometryRendererPrivate();

    Q_DECLARE_PUBLIC(QGeometryRenderer)

    int m_instanceCount;
    int m_vertexCount;
    int m_indexOffset;
    int m_firstInstance;
    int m_firstVertex;
    int m_indexBufferByteOffset;
    int m_restartIndexValue;
    int m_verticesPerPatch;
    bool m_primitiveRestart;
    QGeometry *m_geometry;
    QGeometryRenderer::PrimitiveType m_primitiveType;
    QGeometryFactoryPtr m_geometryFactory;
    float m_sortIndex;
};

struct QGeometryRendererData
{
    int instanceCount;
    int vertexCount;
    int indexOffset;
    int firstInstance;
    int firstVertex;
    int indexBufferByteOffset;
    int restartIndexValue;
    int verticesPerPatch;
    bool primitiveRestart;
    Qt3DCore::QNodeId geometryId;
    QGeometryRenderer::PrimitiveType primitiveType;
    QGeometryFactoryPtr geometryFactory;
};

class QGeometry;
typedef Qt3DCore::QTypedPropertyUpdatedChange<std::unique_ptr<QGeometry>> QGeometryChange;
typedef Qt3DCore::QTypedPropertyUpdatedChangePtr<std::unique_ptr<QGeometry>> QGeometryChangePtr;

} // namespace Qt3DRender

QT_END_NAMESPACE


#endif // QT3DRENDER_QGEOMETRYRENDERER_P_H

