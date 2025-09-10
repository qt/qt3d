// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QRENDERCAPTURE_H
#define QRENDERCAPTURE_H

#include <Qt3DRender/QFrameGraphNode>
#include <QtGui/QImage>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QRenderCapturePrivate;
class QRenderCaptureReplyPrivate;

class Q_3DRENDERSHARED_EXPORT QRenderCaptureReply : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QImage image READ image CONSTANT)
    Q_PROPERTY(int captureId READ captureId CONSTANT)
    Q_PROPERTY(bool complete READ isComplete NOTIFY completed)

public:

    QImage image() const;
    Q_DECL_DEPRECATED int captureId() const;
    bool isComplete() const;

    Q_INVOKABLE bool saveImage(const QString &fileName) const;

Q_SIGNALS:
    void completed();

private:
    Q_DECLARE_PRIVATE(QRenderCaptureReply)

    QRenderCaptureReply(QObject *parent = nullptr);

    friend class QRenderCapturePrivate;
};

class Q_3DRENDERSHARED_EXPORT QRenderCapture : public QFrameGraphNode
{
    Q_OBJECT
public:
    explicit QRenderCapture(Qt3DCore::QNode *parent = nullptr);

    Qt3DRender::QRenderCaptureReply *requestCapture(int captureId);
    Q_REVISION(2, 9) Q_INVOKABLE Qt3DRender::QRenderCaptureReply *requestCapture();
    Q_REVISION(2, 10) Q_INVOKABLE Qt3DRender::QRenderCaptureReply *requestCapture(const QRect &rect);

private:
    Q_DECLARE_PRIVATE(QRenderCapture)
};

} // Qt3DRender

QT_END_NAMESPACE

#endif // QRENDERCAPTURE_H
