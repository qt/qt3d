// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QCOMPUTECOMMAND_P_H
#define QT3DRENDER_QCOMPUTECOMMAND_P_H

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

#include <Qt3DCore/private/qcomponent_p.h>
#include <Qt3DRender/qcomputecommand.h>
#include <Qt3DRender/private/qt3drender_global_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class Q_3DRENDERSHARED_PRIVATE_EXPORT QComputeCommandPrivate : public Qt3DCore::QComponentPrivate
{
public:
    QComputeCommandPrivate();

    int m_workGroupX;
    int m_workGroupY;
    int m_workGroupZ;
    QComputeCommand::RunType m_runType;
    int m_frameCount;

    void setFrameCount(int frameCount);
};

struct QComputeCommandData
{
    int workGroupX;
    int workGroupY;
    int workGroupZ;
    QComputeCommand::RunType runType;
    int frameCount;
};

} // Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QCOMPUTECOMMAND_P_H
