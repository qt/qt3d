// Copyright (C) 2017 Juan José Casafranca
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <Qt3DRender/qbuffercapture.h>
#include <Qt3DRender/private/qbuffercapture_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender
{
//Documentation here

/*!
 * \internal
 */
QBufferCapturePrivate::QBufferCapturePrivate()
    : QFrameGraphNodePrivate()
{

}

/*!
    \class Qt3DRender::QBufferCapture
    \inmodule Qt3DRender
    \brief Exchanges buffer data between GPU and CPU.
*/

/*!
    \qmltype BufferCapture
    \inqmlmodule Qt3D.Render
    \instantiates Qt3DRender::QBufferCapture
    \brief Exchanges buffer data between GPU and CPU.
*/

QBufferCapture::QBufferCapture(Qt3DCore::QNode *parent)
    : QFrameGraphNode(*new QBufferCapturePrivate, parent)
{

}

QBufferCapture::~QBufferCapture()
{
}

} //Qt3DRender

QT_END_NAMESPACE

#include "moc_qbuffercapture.cpp"
