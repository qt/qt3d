// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QRENDERCAPTURE_P_H
#define QRENDERCAPTURE_P_H

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

#include <Qt3DRender/qrendercapture.h>
#include <Qt3DRender/private/qframegraphnode_p.h>

#include <QtCore/qmutex.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

struct QRenderCaptureRequest
{
    int captureId;
    QRect rect;
};

class QRenderCapturePrivate : public QFrameGraphNodePrivate
{
public:
    QRenderCapturePrivate();
    ~QRenderCapturePrivate();
    QList<QRenderCaptureReply *> m_waitingReplies;
    QMutex m_mutex;
    mutable QList<QRenderCaptureRequest> m_pendingRequests;

    QRenderCaptureReply *createReply(int captureId);
    QRenderCaptureReply *takeReply(int captureId);
    void setImage(QRenderCaptureReply *reply, const QImage &image);
    void replyDestroyed(QRenderCaptureReply *reply);

    Q_DECLARE_PUBLIC(QRenderCapture)
};

class QRenderCaptureReplyPrivate : public QObjectPrivate
{
public:
    QRenderCaptureReplyPrivate();

    QImage m_image;
    int m_captureId;
    bool m_complete;


    Q_DECLARE_PUBLIC(QRenderCaptureReply)
};

// used by backend to send render capture to frontend
struct RenderCaptureData
{
    QImage image;
    int captureId;
};

typedef QSharedPointer<RenderCaptureData> RenderCaptureDataPtr;

} // Qt3DRender

QT_END_NAMESPACE

Q_DECLARE_METATYPE(Qt3DRender::RenderCaptureDataPtr) // LCOV_EXCL_LINE
Q_DECLARE_METATYPE(Qt3DRender::QRenderCaptureRequest); // LCOV_EXCL_LINE

#endif // QRENDERCAPTURE_P_H
