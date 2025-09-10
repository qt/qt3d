// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QRENDERTARGET_P_H
#define QT3DRENDER_QRENDERTARGET_P_H

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
#include <Qt3DRender/private/qt3drender_global_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QRenderTargetOutput;
class QRenderTarget;

class Q_3DRENDERSHARED_PRIVATE_EXPORT QRenderTargetPrivate : public Qt3DCore::QComponentPrivate
{
public:
    QRenderTargetPrivate();

    Q_DECLARE_PUBLIC(QRenderTarget)
    QList<QRenderTargetOutput *> m_outputs;
};

struct QRenderTargetData
{
    Qt3DCore::QNodeIdVector outputIds;
};

}

QT_END_NAMESPACE

#endif // QT3DRENDER_QRENDERTARGET_P_H
