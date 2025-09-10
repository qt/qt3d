// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <Qt3DRender/qrendercapture.h>
#include <Qt3DRender/private/qrendercapture_p.h>

#include <QPointer>
#include <QMutexLocker>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

/*!
 * \class Qt3DRender::QRenderCapture
 * \inheaderfile Qt3DRender/QRenderCapture
 * \inmodule Qt3DRender
 *
 * \brief Frame graph node for render capture.
 *
 * The QRenderCapture is used to capture rendering into an image at any render stage.
 * Capturing must be initiated by the user and one image is returned per capture request.
 * User can issue multiple render capture requests simultaniously, but only one request
 * is served per QRenderCapture instance per frame.
 *
 * \since 5.8
 */

/*!
 * \qmltype RenderCapture
 * \nativetype Qt3DRender::QRenderCapture
 * \inherits FrameGraphNode
 * \inqmlmodule Qt3D.Render
 * \since 5.8
 * \brief Capture rendering.
 */

/*!
 * \class Qt3DRender::QRenderCaptureReply
 * \inheaderfile Qt3DRender/QRenderCaptureReply
 * \inmodule Qt3DRender
 *
 * \brief Receives the result of render capture request.
 *
 * An object, which receives the image from QRenderCapture::requestCapture.
 *
 * \since 5.8
 */

/*!
 * \qmltype RenderCaptureReply
 * \nativetype Qt3DRender::QRenderCaptureReply
 * \inqmlmodule Qt3D.Render
 * \since 5.8
 * \brief Receives render capture result.
 */

/*!
 * \qmlproperty variant Qt3D.Render::RenderCaptureReply::image
 *
 * Holds the image, which was produced as a result of render capture.
 */

/*!
 * \qmlproperty int Qt3D.Render::RenderCaptureReply::captureId
 *
 * Holds the captureId, which was passed to the renderCapture.
 */

/*!
 * \qmlproperty bool Qt3D.Render::RenderCaptureReply::complete
 *
 * Holds the complete state of the render capture.
 */

/*!
 * \qmlmethod bool Qt3D.Render::RenderCaptureReply::saveImage(fileName)
 *
 * Saves the render capture result as an image to \a fileName.
 * Returns true if the image was successfully saved; otherwise returns false.
 *
 * \since 5.9
 */

/*!
 * \qmlmethod RenderCaptureReply Qt3D.Render::RenderCapture::requestCapture(int captureId)
 * \deprecated
 *
 * Used to request render capture. User can specify a \a captureId to identify
 * the request. The requestId does not have to be unique. Only one render capture result
 * is produced per requestCapture call even if the frame graph has multiple leaf nodes.
 * The function returns a QRenderCaptureReply object, which receives the captured image
 * when it is done. The user is responsible for deallocating the returned object.
 */

/*!
 * \qmlmethod RenderCaptureReply Qt3D.Render::RenderCapture::requestCapture()
 *
 * Used to request render capture. Only one render capture result is produced per
 * requestCapture call even if the frame graph has multiple leaf nodes.
 * The function returns a QRenderCaptureReply object, which receives the captured image
 * when it is done. The user is responsible for deallocating the returned object.
 */

/*!
 * \qmlmethod RenderCaptureReply Qt3D.Render::RenderCapture::requestCapture(Rect rect)
 *
 * Used to request render capture from a specified \a rect. Only one render capture
 * result is produced per requestCapture call even if the frame graph has multiple leaf nodes.
 * The function returns a QRenderCaptureReply object, which receives the captured image
 * when it is done. The user is responsible for deallocating the returned object.
 */

/*!
 * \internal
 */
QRenderCaptureReplyPrivate::QRenderCaptureReplyPrivate()
    : QObjectPrivate()
    , m_captureId(0)
    , m_complete(false)
{

}

/*!
 * The constructor creates an instance with the specified \a parent.
 */
QRenderCaptureReply::QRenderCaptureReply(QObject *parent)
    : QObject(* new QRenderCaptureReplyPrivate, parent)
{

}

/*!
 * \property Qt3DRender::QRenderCaptureReply::image
 *
 * Holds the image, which was produced as a result of render capture.
 */
QImage QRenderCaptureReply::image() const
{
    Q_D(const QRenderCaptureReply);
    return d->m_image;
}

/*!
 * \property Qt3DRender::QRenderCaptureReply::captureId
 *
 * Holds the captureId, which was passed to the renderCapture.
 */
int QRenderCaptureReply::captureId() const
{
    Q_D(const QRenderCaptureReply);
    return d->m_captureId;
}

/*!
 * \property Qt3DRender::QRenderCaptureReply::complete
 *
 * Holds the complete state of the render capture.
 */
bool QRenderCaptureReply::isComplete() const
{
    Q_D(const QRenderCaptureReply);
    return d->m_complete;
}

/*!
 * Saves the render capture result as an image to \a fileName.
 *
 * Returns true if the image was successfully saved; otherwise returns false.
 * \since 5.9
 */
bool QRenderCaptureReply::saveImage(const QString &fileName) const
{
    Q_D(const QRenderCaptureReply);
    if (d->m_complete)
    {
        return d->m_image.save(fileName);
    }
    return false;
}

/*!
 * \internal
 */
QRenderCapturePrivate::QRenderCapturePrivate()
    : QFrameGraphNodePrivate()
{
}

/*!
 * \internal
 */
QRenderCapturePrivate::~QRenderCapturePrivate()
{
}

/*!
 * \internal
 */
QRenderCaptureReply *QRenderCapturePrivate::createReply(int captureId)
{
    QMutexLocker lock(&m_mutex);
    QRenderCaptureReply *reply = new QRenderCaptureReply();
    reply->d_func()->m_captureId = captureId;
    m_waitingReplies.push_back(reply);
    return reply;
}

/*!
 * \internal
 */
QRenderCaptureReply *QRenderCapturePrivate::takeReply(int captureId)
{
    QRenderCaptureReply *reply = nullptr;
    QMutexLocker lock(&m_mutex);
    for (int i = 0; i < m_waitingReplies.size(); ++i) {
        if (m_waitingReplies[i]->d_func()->m_captureId == captureId) {
            reply = m_waitingReplies.takeAt(i);
            break;
        }
    }
    return reply;
}

/*!
 * \internal
 */
void QRenderCapturePrivate::setImage(QRenderCaptureReply *reply, const QImage &image)
{
    reply->d_func()->m_complete = true;
    reply->d_func()->m_image = image;
}

/*!
 * \internal
 */
void QRenderCapturePrivate::replyDestroyed(QRenderCaptureReply *reply)
{
    QMutexLocker lock(&m_mutex);
    m_waitingReplies.removeAll(reply);
}

/*!
 * The constructor creates an instance with the specified \a parent.
 */
QRenderCapture::QRenderCapture(Qt3DCore::QNode *parent)
    : QFrameGraphNode(*new QRenderCapturePrivate, parent)
{
}

/*!
 * \deprecated Used to request render capture. User can specify a \a captureId
 * to identify the request. The requestId does not have to be unique. Only one
 * render capture result is produced per requestCapture call even if the frame
 * graph has multiple leaf nodes. The function returns a QRenderCaptureReply
 * object, which receives the captured image when it is done. The user is
 * responsible for deallocating the returned object by calling deleteLater().
 */
QRenderCaptureReply *QRenderCapture::requestCapture(int captureId)
{
    Q_D(QRenderCapture);
    QRenderCaptureReply *reply = d->createReply(captureId);
    reply->setParent(this);
    QObject::connect(reply, &QObject::destroyed, this, [&, reply, d] (QObject *) {
        d->replyDestroyed(reply);
    });

    const QRenderCaptureRequest request = { captureId, QRect() };
    d->m_pendingRequests.push_back(request);
    d->update();

    return reply;
}

/*!
 * Used to request render capture from a specified \a rect. Only one render
 * capture result is produced per requestCapture call even if the frame graph
 * has multiple leaf nodes. The function returns a QRenderCaptureReply object,
 * which receives the captured image when it is done. The user is responsible
 * for deallocating the returned object by calling deleteLater().
 */
QRenderCaptureReply *QRenderCapture::requestCapture(const QRect &rect)
{
    Q_D(QRenderCapture);
    static int captureId = 1;
    QRenderCaptureReply *reply = d->createReply(captureId);
    reply->setParent(this);
    QObject::connect(reply, &QObject::destroyed, this, [&, reply, d] (QObject *) {
        d->replyDestroyed(reply);
    });

    const QRenderCaptureRequest request = { captureId, rect };
    d->m_pendingRequests.push_back(request);
    d->update();

    captureId++;

    return reply;
}

/*!
 * Used to request render capture. Only one render capture result is produced
 * per requestCapture call even if the frame graph has multiple leaf nodes. The
 * function returns a QRenderCaptureReply object, which receives the captured
 * image when it is done. The user is responsible for deallocating the returned
 * object by calling deleterLater().
 */
Qt3DRender::QRenderCaptureReply *QRenderCapture::requestCapture()
{
    return requestCapture(QRect());
}

} // Qt3DRender

QT_END_NAMESPACE

#include "moc_qrendercapture.cpp"
