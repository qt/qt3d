/****************************************************************************
**
** Copyright (C) 2015 Paul Lemire paul.lemire350@gmail.com
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

#include "pickeventfilter_p.h"
#include <Qt3DRender/private/qrenderaspect_p.h>

#include <QtGui/QHoverEvent>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

PickEventFilter::PickEventFilter(QRenderAspectPrivate *aspect, QObject *parent)
    : QObject(parent)
    , m_aspect(aspect)
{
}

PickEventFilter::~PickEventFilter()
{
}

/*!
    \internal
    Called from the main thread.
*/
bool PickEventFilter::eventFilter(QObject *obj, QEvent *e)
{
    switch (e->type()) {
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::MouseMove:
        return m_aspect->processMouseEvent(obj, static_cast<QMouseEvent *>(e));
    case QEvent::HoverMove: {
        QHoverEvent *he = static_cast<QHoverEvent *>(e);
        auto mouseEvent = QMouseEvent(QEvent::MouseMove,
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
                                      he->position(),
#else
                                      he->pos(),
#endif
                                      Qt::NoButton, Qt::NoButton,
                                      he->modifiers());
        return m_aspect->processMouseEvent(obj, &mouseEvent);
        }
    case QEvent::KeyPress:
    case QEvent::KeyRelease:
        return m_aspect->processKeyEvent(obj, static_cast<QKeyEvent *>(e));
    default:
        break;
    }

    return false;
}

} // Render

} // Qt3DRender

QT_END_NAMESPACE
