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

#include <Qt3DRender/private/qgeometry_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QAttribute;
class QBuffer;

} // namespace Qt3DRender

namespace Qt3DExtras {

class QCylinderGeometryPrivate : public Qt3DRender::QGeometryPrivate
{
public:
    QCylinderGeometryPrivate();

    void init();

    Q_DECLARE_PUBLIC(QCylinderGeometry)

    int m_rings;
    int m_slices;
    float m_radius;
    float m_length;
    Qt3DRender::QAttribute *m_positionAttribute;
    Qt3DRender::QAttribute *m_normalAttribute;
    Qt3DRender::QAttribute *m_texCoordAttribute;
    Qt3DRender::QAttribute *m_indexAttribute;
    Qt3DRender::QBuffer *m_vertexBuffer;
    Qt3DRender::QBuffer *m_indexBuffer;
};

} // Qt3DExtras

QT_END_NAMESPACE

#endif // QT3DEXTRAS_QCYLINDERGEOMETRY_P_H

