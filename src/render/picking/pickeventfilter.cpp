// Copyright (C) 2015 Paul Lemire paul.lemire350@gmail.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

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
                                      he->position(),
                                      he->globalPosition(),
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

#include "moc_pickeventfilter_p.cpp"
