/****************************************************************************
**
** Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
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

namespace Qt3DRender {
class QGeometryRenderer;
class QGeometry;
class QMaterial;
class QAttribute;
class QBuffer;
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
    Qt3DRender::QGeometry *m_geometry;
    Qt3DRender::QAttribute *m_positionAttr;
    Qt3DRender::QAttribute *m_texCoordAttr;
    Qt3DRender::QAttribute *m_indexAttr;
    Qt3DRender::QBuffer *m_vertexBuffer;
    Qt3DRender::QBuffer *m_indexBuffer;
    QText2DMaterial *m_material;
};

} // namespace Qt3DExtras

QT_END_NAMESPACE

#endif // QT3DEXTRAS_DISTANCEFIELDTEXTRENDERER_P_P_H
