// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// Copyright (C) 2016 The Qt Company Ltd and/or its subsidiary(-ies).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

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
    \nativetype Qt3DRender::QNoDepthMask

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
