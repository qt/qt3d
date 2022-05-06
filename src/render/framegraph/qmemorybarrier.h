/****************************************************************************
**
** Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
******************************************************************************/

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
    Q_ENUM(Operation)
    Q_DECLARE_FLAGS(Operations, Operation)

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

Q_DECLARE_METATYPE(Qt3DRender::QMemoryBarrier::Operations)

#endif // QT3DRENDER_QMEMORYBARRIER_H
