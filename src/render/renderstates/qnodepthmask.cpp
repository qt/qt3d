/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Copyright (C) 2022 The Qt Company Ltd and/or its subsidiary(-ies).
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

#include "qnodepthmask.h"
#include "qrenderstate_p.h"
#include <private/qnode_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

/*!
    \class Qt3DRender::QNoDepthMask
    \brief Disable depth write.
    \since 5.7
    \ingroup renderstates
    \inmodule Qt3DRender

    A Qt3DRender::QNoDepthMask class disables fragment depth write to depth buffer.

    \sa Qt3DRender::QDepthTest
 */

/*!
    \qmltype NoDepthMask
    \brief Disable depth write.
    \since 5.7
    \ingroup renderstates
    \inqmlmodule Qt3D.Render
    \inherits RenderState
    \instantiates Qt3DRender::QNoDepthMask

    A NoDepthMask type disables fragment depth write to depth buffer.

    \sa Qt3DRender::QDepthTest
 */

class QNoDepthMaskPrivate : public QRenderStatePrivate
{
public:
    QNoDepthMaskPrivate()
        : QRenderStatePrivate(Render::DepthWriteStateMask)
    {
    }

    Q_DECLARE_PUBLIC(QNoDepthMask)
};

/*!
    The constructor creates a new QNoDepthMask::QNoDepthMask instance with
    the specified \a parent.
 */
QNoDepthMask::QNoDepthMask(QNode *parent)
    : QRenderState(*new QNoDepthMaskPrivate, parent)
{
}

/*! \internal */
QNoDepthMask::~QNoDepthMask()
{
}

} // namespace Qt3DRender

QT_END_NAMESPACE

#include "moc_qnodepthmask.cpp"
