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

#include "qrenderstate.h"
#include "qrenderstate_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

/*!
    \class Qt3DRender::QRenderState
    \brief An abstract base class for all render states.
    \since 5.7
    \ingroup renderstates
    \inmodule Qt3DRender

    A Qt3DRender::QRenderState class is abstract base class for all render states.
    One can not instantiate QRenderState directly, but through its subclasses.
 */

/*!
    \qmltype RenderState
    \inqmlmodule Qt3D.Render
    \brief An abstract base type for all render states.
    \since 5.7
    \inherits Node
    \instantiates Qt3DRender::QRenderState
    \ingroup renderstates

    A RenderState type is abstract base class for all render states.
    One can not instantiate RenderState directly, but through its subclasses.
 */

/*! \internal */
QRenderStatePrivate::QRenderStatePrivate(Render::StateMask type)
    : QNodePrivate(),
      m_type(type)
{
}

/*! \internal */
QRenderStatePrivate::~QRenderStatePrivate()
{
}

/*! \internal */
QRenderStatePrivate *QRenderStatePrivate::get(QRenderState *state)
{
    return state->d_func();
}

/*! \internal */
const QRenderStatePrivate *QRenderStatePrivate::get(const QRenderState *state)
{
    return state->d_func();
}

/*! \internal */
QRenderState::~QRenderState()
{
}

/*! \internal */
QRenderState::QRenderState(QRenderStatePrivate &dd, QNode *parent)
    : QNode(dd, parent)
{
}

} // namespace Qt3DRender

QT_END_NAMESPACE
