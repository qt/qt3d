// Copyright (C) 2018 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QWAITFENCE_P_H
#define QT3DRENDER_QWAITFENCE_P_H

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

#include <Qt3DRender/private/qframegraphnode_p.h>
#include <Qt3DRender/qwaitfence.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QWaitFencePrivate : public QFrameGraphNodePrivate
{
public:
    QWaitFencePrivate();

    QWaitFence::HandleType m_handleType;
    QVariant m_handle;
    bool m_waitOnCPU;
    quint64 m_timeout;
};

} // Qt3DRender

QT_END_NAMESPACE


#endif // QT3DRENDER_QWAITFENCE_P_H
