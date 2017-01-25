/****************************************************************************
**
** Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
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

#ifndef QT3DRENDER_QMEMORYBARRIER_H
#define QT3DRENDER_QMEMORYBARRIER_H

#include <Qt3DRender/qframegraphnode.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QMemoryBarrierPrivate;

class QT3DRENDERSHARED_EXPORT QMemoryBarrier : public QFrameGraphNode
{
    Q_OBJECT
public:
    explicit QMemoryBarrier(Qt3DCore::QNode *parent = nullptr);
    ~QMemoryBarrier();

    enum BarrierType {
        None = 0,
        VertexAttributeArrayBarrier = (1 << 0),
        ElementArrayBarrier = (1 << 1),
        UniformBarrier = (1 << 2),
        TextureFetchBarrier = (1 << 3),
        ShaderImageAccessBarrier = (1 << 4),
        CommandBarrier = (1 << 5),
        PixelBufferBarrier = (1 << 6),
        TextureUpdateBarrier = (1 << 7),
        BufferUpdateBarrier = (1 << 8),
        FrameBufferBarrier = (1 << 9),
        TransformFeedbackBarrier = (1 << 10),
        AtomicCounterBarrier = (1 << 11),
        ShaderStorageBarrier = (1 << 12),
        QueryBufferBarrier = (1 << 13),
        AllBarrier = 0xFFFFFFFF
    };
    Q_ENUM(BarrierType)
    Q_DECLARE_FLAGS(BarrierTypes, BarrierType)

public Q_SLOTS:
    void setBarrierTypes(QMemoryBarrier::BarrierTypes barrierTypes);
    BarrierTypes barrierTypes() const;

Q_SIGNALS:
    void barrierTypesChanged(QMemoryBarrier::BarrierTypes barrierTypes);

protected:
    explicit QMemoryBarrier(QMemoryBarrierPrivate &dd, Qt3DCore::QNode *parent = nullptr);

private:
    Q_DECLARE_PRIVATE(QMemoryBarrier)
    Qt3DCore::QNodeCreatedChangeBasePtr createNodeCreationChange() const Q_DECL_OVERRIDE;
};

} // Qt3DRender

QT_END_NAMESPACE

Q_DECLARE_METATYPE(Qt3DRender::QMemoryBarrier::BarrierTypes)

#endif // QT3DRENDER_QMEMORYBARRIER_H
