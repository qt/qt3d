/****************************************************************************
**
** Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
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
****************************************************************************/

#include "qseamlesscubemap.h"
#include "qrenderstate_p.h"
#include <private/qnode_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

/*!
    \class Qt3DRender::QSeamlessCubemap
    \brief Enables seamless cubemap texture filtering.
    \since 5.7
    \inmodule Qt3DRender

    When present in a state set, enables the seamless cubemap texture filtering
    as provided by the GL_ARB_seamless_cubemap extension (if available).
 */

/*!
    \qmltype SeamlessCubemap
    \brief Enables seamless cubemap texture filtering.
    \since 5.7
    \inqmlmodule Qt3D.Render
    \instantiates Qt3DRender::QSeamlessCubemap
    \inherits RenderState

    When present in a state set, enables the seamless cubemap texture filtering
    as provided by the GL_ARB_seamless_cubemap extension (if available).
 */


class QSeamlessCubemapPrivate : public QRenderStatePrivate
{
public:
    QSeamlessCubemapPrivate()
        : QRenderStatePrivate(Render::SeamlessCubemapMask)
    {
    }

    Q_DECLARE_PUBLIC(QSeamlessCubemap)
};

/*!
    The constructor creates a new QSeamlessCubemap::QSeamlessCubemap instance
    with the specified \a parent.
 */
QSeamlessCubemap::QSeamlessCubemap(QNode *parent)
    : QRenderState(*new QSeamlessCubemapPrivate, parent)
{
}

/*! \internal */
QSeamlessCubemap::~QSeamlessCubemap()
{
}

} // namespace Qt3DRender

QT_END_NAMESPACE

#include "moc_qseamlesscubemap.cpp"

