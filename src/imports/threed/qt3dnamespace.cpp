/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
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

#include "qt3dnamespace.h"

QT_BEGIN_NAMESPACE

/*!
    \qmltype Qt3D
    \instantiates Qt3DNamespace
    \brief The QML global object Qt3D provides useful functions for 3D applications.
    \ingroup qt3d::qml3d
*/

/*!
    \qmlmethod Qt3D::vector2d(real x, real y)
    Returns a Vector2D with the specified \c x and \c y.
*/

QVector2D Qt3DNamespace::vector2d(float x, float y)
{
    return QVector2D(x, y);
}

/*!
    \qmlmethod Qt3D::vector4d(real x, real y, real z, real w)
    Returns a Vector4D with the specified \c x, \c y, \c z, and \c w.
*/

QVector4D Qt3DNamespace::vector4d(float x, float y, float z, float w)
{
    return QVector4D(x, y, z, w);
}

/*!
    \qmlmethod Qt3D::quaternion(real scalar, real x, real y, real z)
    Returns a Quaternion with the specified \c scalar, \c x, \c y, and \c z.
*/

QQuaternion Qt3DNamespace::quaternion(float scalar, float x, float y, float z)
{
    return QQuaternion(scalar, x, y, z);
}

/*!
    \qmlmethod Qt3D::matrix4x4(real m11, real m12, real m13, real m14, real m21, real m22, real m23, real m24, real m31, real m32, real m33, real m34, real m41, real m42, real m43, real m44)
    Returns a Matrix4x4 with the specified 16 components, which are specified
    in row-major order.
*/

QMatrix4x4 Qt3DNamespace::matrix4x4
    (float m11, float m12, float m13, float m14,
     float m21, float m22, float m23, float m24,
     float m31, float m32, float m33, float m34,
     float m41, float m42, float m43, float m44)
{
    return QMatrix4x4(m11, m12, m13, m14,
                      m21, m22, m23, m24,
                      m31, m32, m33, m34,
                      m41, m42, m43, m44);
}

QT_END_NAMESPACE
