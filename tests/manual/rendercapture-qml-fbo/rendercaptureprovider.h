// Copyright (C) 2016 The Qt Company Ltd and/or its subsidiary(-ies).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#ifndef RENDERCAPTUREPROVIDER_H
#define RENDERCAPTUREPROVIDER_H

#include <QtQuick/QQuickImageProvider>
#include <Qt3DRender/QRenderCapture>

class RenderCaptureProvider : public QQuickImageProvider
{
    Q_OBJECT
public:
    RenderCaptureProvider()
        : QQuickImageProvider(Image)
    {
        m_image = QImage(10,10, QImage::Format_ARGB32);
        m_image.fill(QColor("blue").rgba());
    }

    Q_INVOKABLE void updateImage(Qt3DRender::QRenderCaptureReply *reply)
    {
        m_image = reply->image();
    }

    virtual QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize) override
    {
        Q_UNUSED(id);
        Q_UNUSED(requestedSize);
        *size = m_image.size();
        return m_image;
    }

private:
    QImage m_image;
};

#endif
