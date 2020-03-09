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

#ifndef QT3DCORE_QBOUNDINGVOLUME_P_H
#define QT3DCORE_QBOUNDINGVOLUME_P_H

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
#include <Qt3DCore/qt3dcore_global.h>
#include <private/qcomponent_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {
class QBoundingVolume;
class QGeometryView;

class Q_3DCORESHARED_EXPORT QBoundingVolumePrivate : public Qt3DCore::QComponentPrivate
{
public:
    Q_DECLARE_PUBLIC(QBoundingVolume)

    QBoundingVolumePrivate();

    void setImplicitBounds(const QVector3D &minPoint, const QVector3D &maxPoint, const QVector3D &center, float radius);
    virtual void setView(QGeometryView *view);

    static QBoundingVolumePrivate *get(QBoundingVolume *q);

    QGeometryView *m_view;
    QVector3D m_implicitMinPoint;
    QVector3D m_implicitMaxPoint;
    QVector3D m_implicitCenter;
    QVector3D m_minPoint;
    QVector3D m_maxPoint;
    float m_implicitRadius;
    bool m_implicitPointsValid;
    bool m_explicitPointsValid;
    bool m_primaryProvider;
};

} // namespace Qt3DCore

QT_END_NAMESPACE

#endif // QT3DCORE_QBOUNDINGVOLUME_P_H
