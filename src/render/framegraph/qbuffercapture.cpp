/****************************************************************************
**
** Copyright (C) 2017 Juan José Casafranca
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
