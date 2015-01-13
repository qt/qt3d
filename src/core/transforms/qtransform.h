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

#ifndef QT3D_QTRANSFORM_H
#define QT3D_QTRANSFORM_H

#include <Qt3DCore/qcomponent.h>
#include <Qt3DCore/qt3dcore_global.h>
#include <QAtomicInt>
#include <QMatrix4x4>

QT_BEGIN_NAMESPACE

namespace Qt3D {

class QAbstractTransform;
class QTransformPrivate;

class QT3DCORESHARED_EXPORT QTransform : public QComponent
{
    Q_OBJECT
    Q_PROPERTY(QMatrix4x4 matrix READ matrix NOTIFY matrixChanged)

public:
    explicit QTransform(QNode *parent = 0);
    QTransform(QList<QAbstractTransform *> transforms, QNode *parent = 0);
    QTransform(QAbstractTransform *transform, QNode *parent = 0);

    QMatrix4x4 matrix() const;

    QVector3D rotationCenter() const;
    void setRotationCenter(const QVector3D &rc);

    QList<QAbstractTransform*> transforms() const;

    void addTransform(QAbstractTransform *xform);
    void removeTransform(QAbstractTransform *xform);

Q_SIGNALS:
    void matrixChanged();

protected:
    QTransform(QTransformPrivate &dd, QNode *parent = 0);
    QList<QAbstractTransform *> transformList() const;
    void copy(const QNode *ref) Q_DECL_OVERRIDE;

private:
    Q_DECLARE_PRIVATE(QTransform)
    QT3D_CLONEABLE(QTransform)
    Q_PRIVATE_SLOT(d_func(), void _q_update())
};

} // namespace Qt3D

QT_END_NAMESPACE

#endif // QT3D_QTRANSFORM_H
