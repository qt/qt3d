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

#ifndef QT3DRENDER_QGEOMETRY_H
#define QT3DRENDER_QGEOMETRY_H

#include <Qt3DCore/qnode.h>
#include <Qt3DRender/qt3drender_global.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QAbstractAttribute;
class QGeometryPrivate;
class QBoundingVolumeSpecifier;

typedef QVector<QAbstractAttribute *> QAttributeList;

class QT3DRENDERSHARED_EXPORT QGeometry : public Qt3DCore::QNode
{
    Q_OBJECT
    Q_PROPERTY(int verticesPerPatch READ verticesPerPatch WRITE setVerticesPerPatch NOTIFY verticesPerPatchChanged)
    Q_PROPERTY(Qt3DRender::QBoundingVolumeSpecifier *boundingVolumeSpecifier READ boundingVolumeSpecifier CONSTANT)
public:
    explicit QGeometry(Qt3DCore::QNode *parent = Q_NULLPTR);
    ~QGeometry();

    QAttributeList attributes() const;
    void addAttribute(QAbstractAttribute *attribute);
    void removeAttribute(QAbstractAttribute *attribute);

    int verticesPerPatch() const;

    QBoundingVolumeSpecifier *boundingVolumeSpecifier();

public Q_SLOTS:
    void setVerticesPerPatch(int verticesPerPatch);

Q_SIGNALS:
    void verticesPerPatchChanged(int verticesPerPatch);

protected:
    QGeometry(QGeometryPrivate &dd, Qt3DCore::QNode *parent = Q_NULLPTR);
    void copy(const Qt3DCore::QNode *ref) Q_DECL_OVERRIDE;

private:
    Q_DECLARE_PRIVATE(QGeometry)
    QT3D_CLONEABLE(QGeometry)
    Q_PRIVATE_SLOT(d_func(), void _q_boundingVolumeSpecifierChanged(QAbstractAttribute *))
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QGEOMETRY_H
