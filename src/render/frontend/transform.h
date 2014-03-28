/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QT3D_TRANSFORM_H
#define QT3D_TRANSFORM_H

#include <component.h>
#include <qt3drenderer_global.h>

#include <QMatrix4x4>

namespace Qt3D {

class AbstractTransform;

class QT3DRENDERERSHARED_EXPORT Transform : public Qt3D::Component
{
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<Qt3D::AbstractTransform> transforms READ transformList)
    Q_CLASSINFO("DefaultProperty", "transforms")

public:
    explicit Transform(Node *parent = 0);

    QMatrix4x4 matrix() const;
    void setMatrix(const QMatrix4x4 &m);

    QVector3D rotationCenter() const;
    void setRotationCenter(const QVector3D &rc);

    QList<AbstractTransform*> transforms() const;

    // void insertTransformAt(...)
    Q_INVOKABLE void appendTransfrom(AbstractTransform *xform);
    Q_INVOKABLE void removeTransform(AbstractTransform *xform);

    QQmlListProperty<Qt3D::AbstractTransform> transformList();

private:
    QMatrix4x4 applyTransforms() const;

    static void qmlAppendTransform(QQmlListProperty<Qt3D::AbstractTransform> *list, Qt3D::AbstractTransform *bar);
    static AbstractTransform* transformAt(QQmlListProperty<Qt3D::AbstractTransform> *list, int index);
    static int transformCount(QQmlListProperty<Qt3D::AbstractTransform> *list);
    static void qmlClearTransforms(QQmlListProperty<Qt3D::AbstractTransform> *list);

    mutable bool m_transformsDirty;
    bool m_visible;
    QList<AbstractTransform*> m_transforms;

    mutable QMatrix4x4 m_matrix;
    QMatrix4x4 m_sceneMatrix;
};

} // namespace Qt3D

#endif // QT3D_TRANSFORM_H
