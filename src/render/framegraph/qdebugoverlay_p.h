// Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QDEBUGOVERLAY_P_H
#define QDEBUGOVERLAY_P_H

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

#include <Qt3DRender/qdebugoverlay.h>
#include <Qt3DRender/private/qframegraphnode_p.h>

#include <QtCore/qmutex.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QDebugOverlayPrivate : public QFrameGraphNodePrivate
{
public:
    QDebugOverlayPrivate();
    ~QDebugOverlayPrivate();

    Q_DECLARE_PUBLIC(QDebugOverlay)
};

} // Qt3DRender

QT_END_NAMESPACE

#endif // QDEBUGOVERLAY_P_H
