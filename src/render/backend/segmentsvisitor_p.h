/****************************************************************************
**
** Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
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

#ifndef QT3DRENDER_RENDER_SEGMENTSVISITOR_P_H
#define QT3DRENDER_RENDER_SEGMENTSVISITOR_P_H

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

#include <Qt3DCore/qnodeid.h>
#include <Qt3DCore/private/vector3d_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {
class QEntity;
}

namespace Qt3DRender {

namespace Render {

class GeometryRenderer;
class NodeManagers;

class Q_AUTOTEST_EXPORT SegmentsVisitor
{
public:
    explicit SegmentsVisitor(NodeManagers *manager) : m_manager(manager) { }
    virtual ~SegmentsVisitor();

    void apply(const Qt3DCore::QEntity *entity);
    void apply(const GeometryRenderer *renderer, const Qt3DCore::QNodeId id);

    virtual void visit(uint andx, const Vector3D &a,
                       uint bndx, const Vector3D &b) = 0;

protected:
    NodeManagers *m_manager;
    Qt3DCore::QNodeId m_nodeId;
};

} // namespace Render

} // namespace Qt3DRender

QT_END_NAMESPACE


#endif // QT3DRENDER_RENDER_SEGMENTSVISITOR_P_H
