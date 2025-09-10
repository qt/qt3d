// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DEXTRAS_QPLANEGEOMETRY_P_H
#define QT3DEXTRAS_QPLANEGEOMETRY_P_H

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

#include <QtCore/QSize>

#include <Qt3DCore/private/qgeometry_p.h>
#include <Qt3DExtras/qplanegeometry.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

class QAttribute;
class QBuffer;

} // Qt3DCore

namespace Qt3DExtras {

class QPlaneGeometryPrivate : public Qt3DCore::QGeometryPrivate
{
public:
    QPlaneGeometryPrivate();
    void init();

    float m_width;
    float m_height;
    QSize m_meshResolution;
    bool m_mirrored;
    Qt3DCore::QAttribute *m_positionAttribute;
    Qt3DCore::QAttribute *m_normalAttribute;
    Qt3DCore::QAttribute *m_texCoordAttribute;
    Qt3DCore::QAttribute *m_tangentAttribute;
    Qt3DCore::QAttribute *m_indexAttribute;
    Qt3DCore::QBuffer *m_vertexBuffer;
    Qt3DCore::QBuffer *m_indexBuffer;

    Q_DECLARE_PUBLIC(QPlaneGeometry)

    QByteArray generateVertexData() const;
    QByteArray generateIndexData() const;
};

} // Qt3DExtras

QT_END_NAMESPACE

#endif // QT3DEXTRAS_QPLANEGEOMETRY_P_H

