/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qt3dnamespace.h"

QT_BEGIN_NAMESPACE

/*!
    \qmlclass Qt3D Qt3DNamespace
    \brief The QML global object Qt3D provides useful functions for 3D applications.
    \ingroup qt3d::qml3d
*/

/*!
    \qmlmethod Qt3D::vector2d(real x, real y)
    Returns a Vector2D with the specified \c x and \c y.
*/

QVector2D Qt3DNamespace::vector2d(qreal x, qreal y)
{
    return QVector2D(x, y);
}

/*!
    \qmlmethod Qt3D::vector4d(real x, real y, real z, real w)
    Returns a Vector4D with the specified \c x, \c y, \c z, and \c w.
*/

QVector4D Qt3DNamespace::vector4d(qreal x, qreal y, qreal z, qreal w)
{
    return QVector4D(x, y, z, w);
}

/*!
    \qmlmethod Qt3D::quaternion(real scalar, real x, real y, real z)
    Returns a Quaternion with the specified \c scalar, \c x, \c y, and \c z.
*/

QQuaternion Qt3DNamespace::quaternion(qreal scalar, qreal x, qreal y, qreal z)
{
    return QQuaternion(scalar, x, y, z);
}

/*!
    \qmlmethod Qt3D::matrix4x4(real m11, real m12, real m13, real m14, real m21, real m22, real m23, real m24, real m31, real m32, real m33, real m34, real m41, real m42, real m43, real m44)
    Returns a Matrix4x4 with the specified 16 components, which are specified
    in row-major order.
*/

QMatrix4x4 Qt3DNamespace::matrix4x4
    (qreal m11, qreal m12, qreal m13, qreal m14,
     qreal m21, qreal m22, qreal m23, qreal m24,
     qreal m31, qreal m32, qreal m33, qreal m34,
     qreal m41, qreal m42, qreal m43, qreal m44)
{
    return QMatrix4x4(m11, m12, m13, m14,
                      m21, m22, m23, m24,
                      m31, m32, m33, m34,
                      m41, m42, m43, m44);
}

QT_END_NAMESPACE
