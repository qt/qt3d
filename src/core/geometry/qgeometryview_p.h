/****************************************************************************
**
** Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QT3DCORE_QGEOMETRYVIEW_P_H
#define QT3DCORE_QGEOMETRYVIEW_P_H

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

#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DCore/qgeometryview.h>
#include <Qt3DCore/private/qgeometryfactory_p.h>
#include <Qt3DCore/private/qt3dcore_global_p.h>

#include <QtGui/qvector3d.h>
#include <memory>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

class Q_3DCORESHARED_EXPORT QGeometryViewPrivate : public Qt3DCore::QNodePrivate
{
public:
    QGeometryViewPrivate();
    ~QGeometryViewPrivate();

    static QGeometryViewPrivate *get(QGeometryView *q);

    void update() override;

    Q_DECLARE_PUBLIC(QGeometryView)

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
    QGeometryView::PrimitiveType m_primitiveType;
    bool m_dirty;
};

class BoundingVolumeCalculator
{
public:
    BoundingVolumeCalculator() = default;

    const QVector3D min() const { return m_min; }
    const QVector3D max() const { return m_max; }
    const QVector3D center() const { return m_center; }
    float radius() const { return m_radius; }
    bool isValid() const { return m_radius >= 0.f; }

    bool apply(QAttribute *positionAttribute,
               QAttribute *indexAttribute,
               int drawVertexCount,
               bool primitiveRestartEnabled,
               int primitiveRestartIndex);

private:
    QVector3D m_min;
    QVector3D m_max;
    QVector3D m_center;
    float m_radius = -1.f;
};

} // namespace Qt3DCore

QT_END_NAMESPACE


#endif // QT3DCORE_QGEOMETRYVIEW_P_H

