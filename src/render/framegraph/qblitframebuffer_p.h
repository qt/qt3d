/****************************************************************************
**
** Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
******************************************************************************/

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
