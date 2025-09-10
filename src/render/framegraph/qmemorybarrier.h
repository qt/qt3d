// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QMEMORYBARRIER_H
#define QT3DRENDER_QMEMORYBARRIER_H

#include <Qt3DRender/qframegraphnode.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QMemoryBarrierPrivate;

class Q_3DRENDERSHARED_EXPORT QMemoryBarrier : public QFrameGraphNode
{
    Q_OBJECT
public:
    explicit QMemoryBarrier(Qt3DCore::QNode *parent = nullptr);
    ~QMemoryBarrier();

    enum Operation {
        None = 0,
        VertexAttributeArray = (1 << 0),
        ElementArray = (1 << 1),
        Uniform = (1 << 2),
        TextureFetch = (1 << 3),
        ShaderImageAccess = (1 << 4),
        Command = (1 << 5),
        PixelBuffer = (1 << 6),
        TextureUpdate = (1 << 7),
        BufferUpdate = (1 << 8),
        FrameBuffer = (1 << 9),
        TransformFeedback = (1 << 10),
        AtomicCounter = (1 << 11),
        ShaderStorage = (1 << 12),
        QueryBuffer = (1 << 13),
        All = 0xFFFFFFFF
    };
    Q_DECLARE_FLAGS(Operations, Operation)
    Q_FLAGS(Operations)

    Operations waitOperations() const;

public Q_SLOTS:
    void setWaitOperations(QMemoryBarrier::Operations operations);

Q_SIGNALS:
    void waitOperationsChanged(QMemoryBarrier::Operations barrierTypes);

protected:
    explicit QMemoryBarrier(QMemoryBarrierPrivate &dd, Qt3DCore::QNode *parent = nullptr);

private:
    Q_DECLARE_PRIVATE(QMemoryBarrier)
};

} // Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QMEMORYBARRIER_H
