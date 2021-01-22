/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
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
****************************************************************************/

#include <Qt3DQuickRender/private/quick3drendertargetselector_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {
namespace Quick {

Quick3DRenderTargetSelector::Quick3DRenderTargetSelector(QObject *parent)
    : QObject(parent)
{
}

Quick3DRenderTargetSelector::~Quick3DRenderTargetSelector()
{
}

QVariantList Quick3DRenderTargetSelector::drawBuffers() const
{
    // Converts RenderAttachmentType to int
    QVariantList l;
    const auto outputs = parentRenderTargetSelector()->outputs();
    l.reserve(outputs.size());
    for (const QRenderTargetOutput::AttachmentPoint &b : outputs)
        l.append(static_cast<int>(b));
    return l;
}

void Quick3DRenderTargetSelector::setDrawBuffers(const QVariantList &buffers)
{
    if (buffers != drawBuffers()) {

        // Converts int to RenderAttachmentType
        QVector<QRenderTargetOutput::AttachmentPoint> drawBuffersList;
        drawBuffersList.reserve(buffers.size());
        for (const QVariant &buf : buffers)
            drawBuffersList.append(static_cast<QRenderTargetOutput::AttachmentPoint>(buf.toInt()));

        parentRenderTargetSelector()->setOutputs(drawBuffersList);
        emit drawBuffersChanged();
    }
}

} // namespace Quick
} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
