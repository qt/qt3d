/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QT3DRENDER_RENDER_TRIANGLEBOUNDINGVOLUME_P_H
#define QT3DRENDER_RENDER_TRIANGLEBOUNDINGVOLUME_P_H

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

#include <Qt3DRender/private/qboundingvolume_p.h>
#include <Qt3DCore/qnodeid.h>
#include <QVector3D>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

Q_AUTOTEST_EXPORT bool intersectsSegmentTriangle(const Qt3DCore::QRay3D &ray,
                                                 const QVector3D &a,
                                                 const QVector3D &b,
                                                 const QVector3D &c,
                                                 QVector3D &uvw,
                                                 float &t);

class Q_AUTOTEST_EXPORT TriangleBoundingVolume : public QBoundingVolume
{
public:
    TriangleBoundingVolume();
    explicit TriangleBoundingVolume(const Qt3DCore::QNodeId &id,
                                    const QVector3D &a,
                                    const QVector3D &b,
                                    const QVector3D &c);

    Qt3DCore::QNodeId id() const Q_DECL_FINAL;
    bool intersects(const Qt3DCore::QRay3D &ray, QVector3D *q) const Q_DECL_FINAL;
    Type type() const  Q_DECL_FINAL;

    QVector3D a() const;
    QVector3D b() const;
    QVector3D c() const;

    void setA(const QVector3D &a);
    void setB(const QVector3D &b);
    void setC(const QVector3D &c);

    TriangleBoundingVolume transformed(const QMatrix4x4 &mat) const;

    inline TriangleBoundingVolume &transform(const QMatrix4x4 &mat)
    {
        *this = transformed(mat);
        return *this;
    }

private:
    Qt3DCore::QNodeId m_id;
    QVector3D m_a, m_b, m_c;
};

} // namespace Render

} // namespace Qt3DRender

QT_END_NAMESPACE

Q_DECLARE_METATYPE(Qt3DRender::Render::TriangleBoundingVolume*)

#endif // QT3DRENDER_RENDER_TRIANGLEBOUNDINGVOLUME_P_H
