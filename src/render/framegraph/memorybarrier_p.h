// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_MEMORYBARRIER_H
#define QT3DRENDER_RENDER_MEMORYBARRIER_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <Qt3DRender/qmemorybarrier.h>
#include <Qt3DRender/private/framegraphnode_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

class Q_3DRENDERSHARED_PRIVATE_EXPORT MemoryBarrier : public FrameGraphNode
{
public:
    MemoryBarrier();
    ~MemoryBarrier();

    QMemoryBarrier::Operations waitOperations() const;
    void syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime) override;

private:
    QMemoryBarrier::Operations m_waitOperations;
};

} // Render

} // Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_MEMORYBARRIER_H
