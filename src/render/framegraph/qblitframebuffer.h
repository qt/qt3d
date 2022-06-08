// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QBLITFRAMEBUFFER_H
#define QT3DRENDER_QBLITFRAMEBUFFER_H

#include <Qt3DRender/qframegraphnode.h>
#include <Qt3DRender/qrendertargetoutput.h>
#include <Qt3DRender/qrendertarget.h>
#include <QtCore/QRect>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QBlitFramebufferPrivate;

class Q_3DRENDERSHARED_EXPORT QBlitFramebuffer : public QFrameGraphNode
{
    Q_OBJECT
    Q_PROPERTY(Qt3DRender::QRenderTarget *source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(Qt3DRender::QRenderTarget *destination READ destination WRITE setDestination NOTIFY destinationChanged)
    Q_PROPERTY(QRectF sourceRect READ sourceRect WRITE setSourceRect NOTIFY sourceRectChanged)
    Q_PROPERTY(QRectF destinationRect READ destinationRect WRITE setDestinationRect NOTIFY destinationRectChanged)
    Q_PROPERTY(Qt3DRender::QRenderTargetOutput::AttachmentPoint sourceAttachmentPoint READ sourceAttachmentPoint WRITE setSourceAttachmentPoint NOTIFY sourceAttachmentPointChanged)
    Q_PROPERTY(Qt3DRender::QRenderTargetOutput::AttachmentPoint destinationAttachmentPoint READ destinationAttachmentPoint WRITE setDestinationAttachmentPoint NOTIFY destinationAttachmentPointChanged)
    Q_PROPERTY(InterpolationMethod interpolationMethod READ interpolationMethod WRITE setInterpolationMethod NOTIFY interpolationMethodChanged)
public:
    enum InterpolationMethod {
        Nearest = 0,
        Linear,
    };
    Q_ENUM(InterpolationMethod) // LCOV_EXCL_LINE

    explicit QBlitFramebuffer(Qt3DCore::QNode *parent = nullptr);
    ~QBlitFramebuffer();

    QRenderTarget *source() const;
    QRenderTarget *destination() const;
    QRectF sourceRect() const;
    QRectF destinationRect() const;
    Qt3DRender::QRenderTargetOutput::AttachmentPoint sourceAttachmentPoint() const;
    Qt3DRender::QRenderTargetOutput::AttachmentPoint destinationAttachmentPoint() const;
    InterpolationMethod interpolationMethod() const;

    void setSource(QRenderTarget *source);
    void setDestination(QRenderTarget *destination);
    void setSourceRect(const QRectF &sourceRect);
    void setDestinationRect(const QRectF &destinationRect);
    void setSourceAttachmentPoint(Qt3DRender::QRenderTargetOutput::AttachmentPoint sourceAttachmentPoint);
    void setDestinationAttachmentPoint(Qt3DRender::QRenderTargetOutput::AttachmentPoint destinationAttachmentPoint);
    void setInterpolationMethod(InterpolationMethod interpolationMethod);

Q_SIGNALS:
    void sourceChanged();
    void destinationChanged();
    void sourceRectChanged();
    void destinationRectChanged();
    void sourceAttachmentPointChanged();
    void destinationAttachmentPointChanged();
    void interpolationMethodChanged();

protected:
    explicit QBlitFramebuffer(QBlitFramebufferPrivate &dd, Qt3DCore::QNode *parent = nullptr);

private:
    Q_DECLARE_PRIVATE(QBlitFramebuffer)
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QBLITFRAMEBUFFER_H
