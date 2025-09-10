// Copyright (C) 2018 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_WAITFENCE_P_H
#define QT3DRENDER_RENDER_WAITFENCE_P_H

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

#include <Qt3DRender/qwaitfence.h>
#include <Qt3DRender/private/framegraphnode_p.h>
#include <Qt3DRender/private/qwaitfence_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

class Q_AUTOTEST_EXPORT WaitFence : public FrameGraphNode
{
public:
    WaitFence();
    ~WaitFence();

    struct Data
    {
        QWaitFence::HandleType handleType;
        QVariant handle;
        bool waitOnCPU;
        quint64 timeout;
    };

    inline Data data() const { return m_data; }
    void syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime) override;

private:
    Data m_data;
};

} // namespace Render

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_WAITFENCE_P_H
