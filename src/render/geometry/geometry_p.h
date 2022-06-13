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

#ifndef QT3DRENDER_RENDER_GEOMETRY_H
#define QT3DRENDER_RENDER_GEOMETRY_H

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

#include <Qt3DRender/private/backendnode_p.h>


QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

class Q_3DRENDERSHARED_PRIVATE_EXPORT Geometry : public BackendNode
{
public:
    Geometry();
    ~Geometry();

    void cleanup();

    void syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime) override;

    inline QVector<Qt3DCore::QNodeId> attributes() const { return m_attributes; }
    inline bool isDirty() const { return m_geometryDirty; }
    inline Qt3DCore::QNodeId boundingPositionAttribute() const { return m_boundingPositionAttribute; }
    void unsetDirty();

    inline QVector3D min() const { return m_min; }
    inline QVector3D max() const { return m_max; }

    void updateExtent(const QVector3D &min, const QVector3D &max);

private:
    QVector<Qt3DCore::QNodeId> m_attributes;
    bool m_geometryDirty;
    Qt3DCore::QNodeId m_boundingPositionAttribute;
    QVector3D m_min;
    QVector3D m_max;
};

} // namespace Render

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_GEOMETRY_H
