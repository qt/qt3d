// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DCORE_QBUFFER_H
#define QT3DCORE_QBUFFER_H

#include <Qt3DCore/qnode.h>
#include <Qt3DCore/qt3dcore_global.h>
#include <QtCore/QSharedPointer>


QT_BEGIN_NAMESPACE

namespace Qt3DCore {

class QBufferPrivate;

class Q_3DCORESHARED_EXPORT QBuffer : public Qt3DCore::QNode
{
    Q_OBJECT
    Q_PROPERTY(UsageType usage READ usage WRITE setUsage NOTIFY usageChanged)
    Q_PROPERTY(AccessType accessType READ accessType WRITE setAccessType NOTIFY accessTypeChanged QT3D_PROPERTY_REVISION(2, 9))

public:
    enum UsageType
    {
        StreamDraw          = 0x88E0, // GL_STREAM_DRAW
        StreamRead          = 0x88E1, // GL_STREAM_READ
        StreamCopy          = 0x88E2, // GL_STREAM_COPY
        StaticDraw          = 0x88E4, // GL_STATIC_DRAW
        StaticRead          = 0x88E5, // GL_STATIC_READ
        StaticCopy          = 0x88E6, // GL_STATIC_COPY
        DynamicDraw         = 0x88E8, // GL_DYNAMIC_DRAW
        DynamicRead         = 0x88E9, // GL_DYNAMIC_READ
        DynamicCopy         = 0x88EA  // GL_DYNAMIC_COPY
    };
    Q_ENUM(UsageType) // LCOV_EXCL_LINE

    enum AccessType {
        Write = 0x1,
        Read = 0x2,
        ReadWrite = Write|Read
    };
    Q_ENUM(AccessType) // LCOV_EXCL_LINE

    explicit QBuffer(Qt3DCore::QNode *parent = nullptr);
    ~QBuffer();

    UsageType usage() const;
    AccessType accessType() const;

    void setData(const QByteArray &bytes);
    QByteArray data() const;

    Q_INVOKABLE void updateData(int offset, const QByteArray &bytes);

public Q_SLOTS:
    void setUsage(UsageType usage);
    void setAccessType(AccessType access);

Q_SIGNALS:
    void dataChanged(const QByteArray &bytes);
    void usageChanged(UsageType usage);
    void accessTypeChanged(AccessType access);
    void dataAvailable();

private:
    Q_DECLARE_PRIVATE(QBuffer)
};

} // namespace Qt3DCore

QT_END_NAMESPACE

#endif // QT3DCORE_QBUFFER_H
