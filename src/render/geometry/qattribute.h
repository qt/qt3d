/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
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

#ifndef QT3DRENDER_QATTRIBUTE_H
#define QT3DRENDER_QATTRIBUTE_H

#include <Qt3DRender/qt3drender_global.h>
#include <Qt3DCore/QNode>
#include <QtCore/QSharedPointer>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QBuffer;
class QAttributePrivate;

typedef QSharedPointer<QBuffer> QBufferPtr;

class QT3DRENDERSHARED_EXPORT QAttribute : public Qt3DCore::QNode
{
    Q_OBJECT
    Q_PROPERTY(Qt3DRender::QBuffer *buffer READ buffer WRITE setBuffer NOTIFY bufferChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(DataType dataType READ dataType WRITE setDataType NOTIFY dataTypeChanged)
    Q_PROPERTY(uint dataSize READ dataSize WRITE setDataSize NOTIFY dataSizeChanged)
    Q_PROPERTY(uint count READ count WRITE setCount NOTIFY countChanged)
    Q_PROPERTY(uint byteStride READ byteStride WRITE setByteStride NOTIFY byteStrideChanged)
    Q_PROPERTY(uint byteOffset READ byteOffset WRITE setByteOffset NOTIFY byteOffsetChanged)
    Q_PROPERTY(uint divisor READ divisor WRITE setDivisor NOTIFY divisorChanged)
    Q_PROPERTY(AttributeType attributeType READ attributeType WRITE setAttributeType NOTIFY attributeTypeChanged)

public:
    enum AttributeType {
        VertexAttribute,
        IndexAttribute
    };

    Q_ENUM(AttributeType)

    enum DataType {
        Byte = 0,
        UnsignedByte,
        Short,
        UnsignedShort,
        Int,
        UnsignedInt,
        HalfFloat,
        Float,
        Double
    };
    Q_ENUM(DataType)

    explicit QAttribute(QNode *parent = 0);
    explicit QAttribute(QBuffer *buf, DataType dataType, uint dataSize, uint count, uint offset = 0, uint stride = 0, QNode *parent = 0);
    explicit QAttribute(QBuffer *buf, const QString &name, DataType dataType, uint dataSize, uint count, uint offset = 0, uint stride = 0, QNode *parent = 0);
    ~QAttribute();

    QBuffer *buffer() const;
    QString name() const;
    DataType dataType() const;
    uint dataSize() const;
    uint count() const;
    uint byteStride() const;
    uint byteOffset() const;
    uint divisor() const;
    AttributeType attributeType() const;

    Q_INVOKABLE static QString defaultPositionAttributeName();
    Q_INVOKABLE static QString defaultNormalAttributeName();
    Q_INVOKABLE static QString defaultColorAttributeName();
    Q_INVOKABLE static QString defaultTextureCoordinateAttributeName();
    Q_INVOKABLE static QString defaultTangentAttributeName();

public Q_SLOTS:
    void setBuffer(QBuffer *buffer);
    void setName(const QString &name);
    void setDataType(DataType type);
    void setDataSize(uint size);
    void setCount(uint count);
    void setByteStride(uint byteStride);
    void setByteOffset(uint byteOffset);
    void setDivisor(uint divisor);
    void setAttributeType(AttributeType attributeType);

Q_SIGNALS:
    void bufferChanged(QBuffer *buffer);
    void nameChanged(const QString &name);
    void dataTypeChanged(DataType dataType);
    void dataSizeChanged(uint dataSize);
    void countChanged(uint count);
    void byteStrideChanged(uint byteStride);
    void byteOffsetChanged(uint byteOffset);
    void divisorChanged(uint divisor);
    void attributeTypeChanged(AttributeType attributeType);

protected:
    void copy(const QNode *ref) Q_DECL_OVERRIDE;

private:
    Q_DECLARE_PRIVATE(QAttribute)
    QT3D_CLONEABLE(QAttribute)
};

} // Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QATTRIBUTE_H
