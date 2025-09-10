// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// Copyright (C) 2016 The Qt Company Ltd and/or its subsidiary(-ies).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QRENDERSTATE_P_H
#define QT3DRENDER_QRENDERSTATE_P_H

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

#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DRender/qrenderstate.h>
#include <Qt3DRender/private/qt3drender_global_p.h>
#include <Qt3DRender/private/statemask_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class Q_3DRENDERSHARED_PRIVATE_EXPORT QRenderStatePrivate : public Qt3DCore::QNodePrivate
{
public:
    explicit QRenderStatePrivate(Render::StateMask type);
    ~QRenderStatePrivate();


    Q_DECLARE_PUBLIC(QRenderState)


    const Render::StateMask m_type;

    static QRenderStatePrivate *get(QRenderState *state);
    static const QRenderStatePrivate *get(const QRenderState *state);
};

}

QT_END_NAMESPACE

#endif // QT3DRENDER_QRENDERSTATE_P_H
