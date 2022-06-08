// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DCORE_QATTRIBUTE_H
#define QT3DCORE_QATTRIBUTE_H

#include <Qt3DCore/qt3dcore_global.h>
#include <Qt3DCore/qnode.h>
#include <Qt3DCore/qbuffer.h>
#include <QtCore/QSharedPointer>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

class QAttributePrivate;

typedef QSharedPointer<QBuffer> QBufferPtr;

class Q_3DCORESHARED_EXPORT QAttribute : public QNode
{
    Q_OBJECT
    Q_PROPERTY(Qt3DCore::QBuffer *buffer READ buffer WRITE setBuffer NOTIFY bufferChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(VertexBaseType vertexBaseType READ vertexBaseType WRITE setVertexBaseType NOTIFY vertexBaseTypeChanged)
    Q_PROPERTY(uint vertexSize READ vertexSize WRITE setVertexSize NOTIFY vertexSizeChanged)
    Q_PROPERTY(uint count READ count WRITE setCount NOTIFY countChanged)
    Q_PROPERTY(uint byteStride READ byteStride WRITE setByteStride NOTIFY byteStrideChanged)
    Q_PROPERTY(uint byteOffset READ byteOffset WRITE setByteOffset NOTIFY byteOffsetChanged)
    Q_PROPERTY(uint divisor READ divisor WRITE setDivisor NOTIFY divisorChanged)
    Q_PROPERTY(AttributeType attributeType READ attributeType WRITE setAttributeType NOTIFY attributeTypeChanged)
    Q_PROPERTY(QString defaultPositionAttributeName READ defaultPositionAttributeName CONSTANT)
    Q_PROPERTY(QString defaultNormalAttributeName READ defaultNormalAttributeName CONSTANT)
    Q_PROPERTY(QString defaultColorAttributeName READ defaultColorAttributeName CONSTANT)
    Q_PROPERTY(QString defaultTextureCoordinateAttributeName READ defaultTextureCoordinateAttributeName CONSTANT)
    Q_PROPERTY(QString defaultTextureCoordinate1AttributeName READ defaultTextureCoordinate1AttributeName CONSTANT QT3D_PROPERTY_REVISION(2, 11))
    Q_PROPERTY(QString defaultTextureCoordinate2AttributeName READ defaultTextureCoordinate2AttributeName CONSTANT QT3D_PROPERTY_REVISION(2, 11))
    Q_PROPERTY(QString defaultTangentAttributeName READ defaultTangentAttributeName CONSTANT)
    Q_PROPERTY(QString defaultJointIndicesAttributeName READ defaultJointIndicesAttributeName CONSTANT QT3D_PROPERTY_REVISION(2, 10))
    Q_PROPERTY(QString defaultJointWeightsAttributeName READ defaultJointWeightsAttributeName CONSTANT QT3D_PROPERTY_REVISION(2, 10))

public:
    enum AttributeType {
        VertexAttribute,
        IndexAttribute,
        DrawIndirectAttribute
    };

    Q_ENUM(AttributeType) // LCOV_EXCL_LINE

    enum VertexBaseType {
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
    Q_ENUM(VertexBaseType) // LCOV_EXCL_LINE

    explicit QAttribute(QNode *parent = nullptr);
    explicit QAttribute(QBuffer *buf, VertexBaseType vertexBaseType, uint vertexSize, uint count, uint offset = 0, uint stride = 0, QNode *parent = nullptr);
    explicit QAttribute(QBuffer *buf, const QString &name, VertexBaseType vertexBaseType, uint vertexSize, uint count, uint offset = 0, uint stride = 0, QNode *parent = nullptr);
    ~QAttribute();

    QBuffer *buffer() const;
    QString name() const;
    VertexBaseType vertexBaseType() const;
    uint vertexSize() const;
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
    static QString defaultJointIndicesAttributeName();
    static QString defaultJointWeightsAttributeName();
    static QString defaultTextureCoordinate1AttributeName();
    static QString defaultTextureCoordinate2AttributeName();

public Q_SLOTS:
    void setBuffer(QBuffer *buffer);
    void setName(const QString &name);
    void setVertexBaseType(VertexBaseType type);
    void setVertexSize(uint size);
    void setCount(uint count);
    void setByteStride(uint byteStride);
    void setByteOffset(uint byteOffset);
    void setDivisor(uint divisor);
    void setAttributeType(AttributeType attributeType);

Q_SIGNALS:
    void bufferChanged(QBuffer *buffer);
    void nameChanged(const QString &name);
    void vertexBaseTypeChanged(VertexBaseType vertexBaseType);
    void vertexSizeChanged(uint vertexSize);
    void dataTypeChanged(VertexBaseType vertexBaseType);
    void dataSizeChanged(uint vertexSize);
    void countChanged(uint count);
    void byteStrideChanged(uint byteStride);
    void byteOffsetChanged(uint byteOffset);
    void divisorChanged(uint divisor);
    void attributeTypeChanged(AttributeType attributeType);

private:
    Q_DECLARE_PRIVATE(QAttribute)
};

} // Qt3DCore

QT_END_NAMESPACE

#endif // QT3DCORE_QATTRIBUTE_H
