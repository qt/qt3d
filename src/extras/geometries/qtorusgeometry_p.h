// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DEXTRAS_QTORUSGEOMETRY_P_H
#define QT3DEXTRAS_QTORUSGEOMETRY_P_H

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

#include <Qt3DCore/private/qgeometry_p.h>
#include <Qt3DExtras/qtorusgeometry.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

class QAttribute;
class QBuffer;

} // Qt3DCore

namespace Qt3DExtras {

class QTorusGeometryPrivate : public Qt3DCore::QGeometryPrivate
{
public:
    QTorusGeometryPrivate();
    void init();

    int m_rings;
    int m_slices;
    float m_radius;
    float m_minorRadius;
    Qt3DCore::QAttribute *m_positionAttribute;
    Qt3DCore::QAttribute *m_normalAttribute;
    Qt3DCore::QAttribute *m_texCoordAttribute;
    Qt3DCore::QAttribute *m_tangentAttribute;
    Qt3DCore::QAttribute *m_indexAttribute;
    Qt3DCore::QBuffer *m_vertexBuffer;
    Qt3DCore::QBuffer *m_indexBuffer;

    Q_DECLARE_PUBLIC(QTorusGeometry)

    QByteArray generateVertexData() const;
    QByteArray generateIndexData() const;
};

} // Qt3DExtras


QT_END_NAMESPACE

#endif // QT3DEXTRAS_QTORUSGEOMETRY_P_H

