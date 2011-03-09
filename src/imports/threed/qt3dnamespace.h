/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtQuick3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QT3DNAMESPACE_H
#define QT3DNAMESPACE_H

#include <QtCore/qobject.h>
#include <QtCore/qvariant.h>
#include <QtGui/qvector2d.h>
#include <QtGui/qvector3d.h>
#include <QtGui/qvector4d.h>
#include <QtGui/qmatrix4x4.h>
#include <QtGui/qquaternion.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class Qt3DNamespace : public QObject
{
    Q_OBJECT
public:
    Qt3DNamespace(QObject *parent = 0) : QObject(parent) {}
    ~Qt3DNamespace() {}

public Q_SLOTS:
    QVector2D vector2d(qreal x, qreal y);
    QVector4D vector4d(qreal x, qreal y, qreal z, qreal w);
    QQuaternion quaternion(qreal scalar, qreal x, qreal y, qreal z);
    QMatrix4x4 matrix4x4(qreal m11, qreal m12, qreal m13, qreal m14,
                         qreal m21, qreal m22, qreal m23, qreal m24,
                         qreal m31, qreal m32, qreal m33, qreal m34,
                         qreal m41, qreal m42, qreal m43, qreal m44);
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QT3DNAMESPACE_H
