// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QBLENDEQUATIONARGUMENTS_P_H
#define QT3DRENDER_QBLENDEQUATIONARGUMENTS_P_H

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

#include <Qt3DRender/private/qrenderstate_p.h>
#include <Qt3DRender/qblendequationarguments.h>
#include <Qt3DRender/private/qt3drender_global_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class Q_3DRENDERSHARED_PRIVATE_EXPORT QBlendEquationArgumentsPrivate : public QRenderStatePrivate
{
public:
    QBlendEquationArgumentsPrivate(Render::StateMask type = Render::BlendEquationArgumentsMask)
        : QRenderStatePrivate(type)
        , m_sourceRgb(QBlendEquationArguments::Zero)
        , m_sourceAlpha(QBlendEquationArguments::One)
        , m_destinationRgb(QBlendEquationArguments::Zero)
        , m_destinationAlpha(QBlendEquationArguments::Zero)
        , m_bufferIndex(-1)
    {
    }

    Q_DECLARE_PUBLIC(QBlendEquationArguments)

    QBlendEquationArguments::Blending m_sourceRgb;
    QBlendEquationArguments::Blending m_sourceAlpha;
    QBlendEquationArguments::Blending m_destinationRgb;
    QBlendEquationArguments::Blending m_destinationAlpha;
    int m_bufferIndex;
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QBLENDEQUATIONARGUMENTS_P_H
