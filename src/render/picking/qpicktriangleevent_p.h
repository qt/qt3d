// Copyright (C) 2019 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

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

#ifndef QT3DRENDER_QPICKTRIANGLEEVENT_P_H
#define QT3DRENDER_QPICKTRIANGLEEVENT_P_H

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

#include <Qt3DRender/private/qpickevent_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
class QPickTriangleEvent;

class Q_3DRENDERSHARED_PRIVATE_EXPORT QPickTriangleEventPrivate : public QPickEventPrivate
{
public:
    QPickTriangleEventPrivate();

    static const QPickTriangleEventPrivate *get(const QPickTriangleEvent *ev);
    QPickTriangleEvent *clone() const;

    uint m_triangleIndex;
    uint m_vertex1Index;
    uint m_vertex2Index;
    uint m_vertex3Index;
    QVector3D m_uvw;
};

} // Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QPICKTRIANGLEEVENT_P_H
