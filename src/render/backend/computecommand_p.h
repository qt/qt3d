// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_COMPUTECOMMAND_P_H
#define QT3DRENDER_RENDER_COMPUTECOMMAND_P_H

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

#include <Qt3DRender/private/backendnode_p.h>
#include <Qt3DRender/qcomputecommand.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

class ComputeJobPrivate;

class Q_3DRENDERSHARED_PRIVATE_EXPORT ComputeCommand : public BackendNode
{
public:
    ComputeCommand();
    ~ComputeCommand();

    void cleanup();
    void syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime) override;

    inline int x() const noexcept { return m_workGroups[0]; }
    inline int y() const noexcept { return m_workGroups[1]; }
    inline int z() const noexcept { return m_workGroups[2]; }
    inline int frameCount() const noexcept { return m_frameCount; }
    inline QComputeCommand::RunType runType() const noexcept { return m_runType; }

    // Called from a job
    void updateFrameCount();
    bool hasReachedFrameCount() const;
    void resetHasReachedFrameCount();

private:
    int m_workGroups[3];
    int m_frameCount;
    QComputeCommand::RunType m_runType;
    bool m_hasReachedFrameCount;
};

} // Render

} // Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_COMPUTECOMMAND_P_H
