// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// Copyright (C) 2016 The Qt Company Ltd and/or its subsidiary(-ies).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QRENDERPASS_P_H
#define QT3DRENDER_QRENDERPASS_P_H

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

#include <private/qnode_p.h>
#include <Qt3DRender/qrenderpass.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QParameter;
class QRenderState;
class QFilterKey;

class QRenderPassPrivate : public Qt3DCore::QNodePrivate
{
public:
    QRenderPassPrivate();

    Q_DECLARE_PUBLIC(QRenderPass)

    QList<QFilterKey *> m_filterKeyList;
    QList<QRenderState *> m_renderStates;
    QList<QParameter *> m_parameters;
    QShaderProgram *m_shader;
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QRENDERPASS_P_H
