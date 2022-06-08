// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QVIEWPORT_P_H
#define QT3DRENDER_QVIEWPORT_P_H

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

#include <private/qframegraphnode_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QViewport;

class QViewportPrivate : public QFrameGraphNodePrivate
{
public :
    QViewportPrivate();

    Q_DECLARE_PUBLIC(QViewport)
    QRectF m_normalizedRect;
    QColor m_clearColor;
    float m_gamma;
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QVIEWPORT_P_H
