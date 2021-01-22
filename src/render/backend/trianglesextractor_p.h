/****************************************************************************
**
** Copyright (C) 2015 Paul Lemire paul.lemire350@gmail.com
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

#ifndef QT3DRENDER_RENDER_TRIANGLESEXTRACTOR_P_H
#define QT3DRENDER_RENDER_TRIANGLESEXTRACTOR_P_H

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

#include "trianglesvisitor_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace RayCasting {
class QBoundingVolume;
}

namespace Render {

class Q_AUTOTEST_EXPORT TrianglesExtractor : private TrianglesVisitor
{
public:
    explicit TrianglesExtractor(GeometryRenderer *renderer,
                                NodeManagers *manager);

    QVector<RayCasting::QBoundingVolume *> extract(const Qt3DCore::QNodeId id);

private:
    void visit(uint andx, const Vector3D &a,
               uint bndx, const Vector3D &b,
               uint cndx, const Vector3D &c) override;

    GeometryRenderer *m_renderer;
    QVector<RayCasting::QBoundingVolume *> m_volumes;
};

} // namespace Render

} // namespace Qt3DRender

QT_END_NAMESPACE


#endif // QT3DRENDER_RENDER_TRIANGLESEXTRACTOR_P_H
