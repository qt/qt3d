// Copyright (C) 2016 The Qt Company Ltd and/or its subsidiary(-ies).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#ifndef MYCAPTURE_H
#define MYCAPTURE_H

#include <QLabel>
#include <Qt3DRender/QRenderCapture>

class MyCapture : public QObject
{
    Q_OBJECT
public:
    MyCapture(Qt3DRender::QRenderCapture* capture, QLabel *imageLabel)
        : m_capture(capture)
        , m_reply(nullptr)
        , m_imageLabel(imageLabel)
        , m_continuous(false)
    {
    }

public slots:
    void onCompleted()
    {
        QObject::disconnect(connection);

        m_imageLabel->setPixmap(QPixmap::fromImage(m_reply->image()));

        m_reply->saveImage("capture.bmp");

        m_reply->deleteLater();
        m_reply = nullptr;

        if (m_continuous)
            capture();
    }

    void setContinuous(bool continuos)
    {
        m_continuous = continuos;
    }

    void capture()
    {
        if (!m_reply) {
            m_reply = m_capture->requestCapture();
            connection = QObject::connect(m_reply, &Qt3DRender::QRenderCaptureReply::completed,
                                          this, &MyCapture::onCompleted);
        }
    }

private:
    Qt3DRender::QRenderCapture* m_capture;
    Qt3DRender::QRenderCaptureReply *m_reply;
    QMetaObject::Connection connection;
    QLabel *m_imageLabel;
    bool m_continuous;
};

#endif

