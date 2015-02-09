/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
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

#ifndef QT3D_QABSTRACTATTRIBUTE_H
#define QT3D_QABSTRACTATTRIBUTE_H

#include <Qt3DCore/qt3dcore_global.h>
#include <QSharedPointer>

QT_BEGIN_NAMESPACE

namespace Qt3D {

class QAbstractBuffer;
class QAbstractAttributePrivate;

typedef QSharedPointer<QAbstractBuffer> QAbstractBufferPtr;

class QT3DCORESHARED_EXPORT QAbstractAttribute
{
public:
    explicit QAbstractAttribute(QAbstractBufferPtr buf, int type, uint count, uint offset = 0, uint stride = 0);
    virtual ~QAbstractAttribute();

    int type() const;
    uint count() const;
    uint byteStride() const;
    uint byteOffset();
    uint divisor() const;
    QAbstractBufferPtr buffer() const;

    virtual QVector<QVector3D> asVector3D() const = 0;
    virtual QVector<QVector2D> asVector2D() const = 0;
    virtual void dump(int count) = 0;

protected:
    Q_DECLARE_PRIVATE(QAbstractAttribute)
    QAbstractAttributePrivate *d_ptr;
    QAbstractAttribute(QAbstractAttributePrivate &dd, QAbstractBufferPtr buf, int type, uint count, uint offset = 0, uint stride = 0);
};

} // Qt3D

QT_END_NAMESPACE

#endif // QT3D_QABSTRACTATTRIBUTE_H
