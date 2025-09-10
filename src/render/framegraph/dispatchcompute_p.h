// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_DISPATCHCOMPUTE_H
#define QT3DRENDER_RENDER_DISPATCHCOMPUTE_H

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

#include <Qt3DRender/private/framegraphnode_p.h>
#include <Qt3DRender/qdispatchcompute.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

class Q_3DRENDERSHARED_PRIVATE_EXPORT DispatchCompute : public FrameGraphNode
{
public:
    DispatchCompute();
    ~DispatchCompute();

    void cleanup();

    inline int x() const noexcept { return m_workGroups[0]; }
    inline int y() const noexcept { return m_workGroups[1]; }
    inline int z() const noexcept { return m_workGroups[2]; }

    void syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime) override;

private:

    int m_workGroups[3];
};

} // Render

} // Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_DISPATCHCOMPUTE_H
