// Copyright (C) 2017 Juan José Casafranca
// SPDX-License-Identifier: LicenseRef-Qt-Commercial

#ifndef QT3DRENDER_QBUFFERCAPTURE_H
#define QT3DRENDER_QBUFFERCAPTURE_H

#include <Qt3DRender/QFrameGraphNode>
#include <Qt3DCore/QBuffer>
#include <QtCore/QByteArray>

QT_BEGIN_NAMESPACE

namespace Qt3DRender
{

class QBufferCapturePrivate;

class Q_3DRENDERSHARED_EXPORT QBufferCapture : public QFrameGraphNode
{
    Q_OBJECT
public:
    explicit QBufferCapture(Qt3DCore::QNode *parent = nullptr);
    ~QBufferCapture();

private:
    Q_DECLARE_PRIVATE(QBufferCapture)
};

} //Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QBUFFERCAPTURE_H
