// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QBLITFRAMEBUFFER_P_H
#define QT3DRENDER_QBLITFRAMEBUFFER_P_H

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
#include <Qt3DRender/qblitframebuffer.h>

#include <Qt3DRender/QRenderTarget>
#include <QRect>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QBlitFramebufferPrivate : public QFrameGraphNodePrivate
{
public:
    QBlitFramebufferPrivate();

    QRenderTarget *m_source;
    QRenderTarget *m_destination;
    QRect m_sourceRect;
    QRect m_destinationRect;
    Qt3DRender::QRenderTargetOutput::AttachmentPoint m_sourceAttachmentPoint;
    Qt3DRender::QRenderTargetOutput::AttachmentPoint m_destinationAttachmentPoint;
    QBlitFramebuffer::InterpolationMethod m_interpolationMethod;

    Q_DECLARE_PUBLIC(QBlitFramebuffer)
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QBLITFRAMEBUFFER_P_H
