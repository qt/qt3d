// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QRENDERPASSFILTER_P_H
#define QT3DRENDER_QRENDERPASSFILTER_P_H

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

class QRenderPassFilter;

class QRenderPassFilterPrivate : public QFrameGraphNodePrivate
{
public:
    QRenderPassFilterPrivate()
        : QFrameGraphNodePrivate()
    {}

    Q_DECLARE_PUBLIC(QRenderPassFilter)
    QString m_renderPassName;
    QList<QFilterKey *> m_matchList;
    QList<QParameter *> m_parameters;
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QRENDERPASSFILTER_P_H
