// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DEXTRAS_QCYLINDERGEOMETRY_P_H
#define QT3DEXTRAS_QCYLINDERGEOMETRY_P_H

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

#include <Qt3DCore/private/qgeometry_p.h>
#include <Qt3DExtras/qcylindergeometry.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

class QAttribute;
class QBuffer;

} // namespace Qt3DCore

namespace Qt3DExtras {

class QCylinderGeometryPrivate : public Qt3DCore::QGeometryPrivate
{
public:
    QCylinderGeometryPrivate();

    void init();

    Q_DECLARE_PUBLIC(QCylinderGeometry)

    int m_rings;
    int m_slices;
    float m_radius;
    float m_length;
    Qt3DCore::QAttribute *m_positionAttribute;
    Qt3DCore::QAttribute *m_normalAttribute;
    Qt3DCore::QAttribute *m_texCoordAttribute;
    Qt3DCore::QAttribute *m_indexAttribute;
    Qt3DCore::QBuffer *m_vertexBuffer;
    Qt3DCore::QBuffer *m_indexBuffer;

    QByteArray generateVertexData() const;
    QByteArray generateIndexData() const;
};

} // Qt3DExtras

QT_END_NAMESPACE

#endif // QT3DEXTRAS_QCYLINDERGEOMETRY_P_H

