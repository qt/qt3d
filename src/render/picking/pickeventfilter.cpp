/****************************************************************************
**
** Copyright (C) 2015 Paul Lemire paul.lemire350@gmail.com
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "pickeventfilter_p.h"

#include <QtCore/QMutexLocker>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

PickEventFilter::PickEventFilter(QObject *parent)
    : QObject(parent)
{
}

PickEventFilter::~PickEventFilter()
{
}

/*!
    \internal
    Called from a worker thread in the thread pool so be sure to
    mutex protect the data.
*/
QList<QMouseEvent> PickEventFilter::pendingEvents()
{
    QMutexLocker locker(&m_mutex);
    QList<QMouseEvent> pendingEvents(m_pendingEvents);
    m_pendingEvents.clear();
    return pendingEvents;
}

/*!
    \internal
    Called from the main thread.
*/
bool PickEventFilter::eventFilter(QObject *obj, QEvent *e)
{
    Q_UNUSED(obj);
    switch (e->type()) {
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::MouseMove:
    case QEvent::HoverMove:
    case Qt::TapGesture: {
        QMutexLocker locker(&m_mutex);
        m_pendingEvents.push_back(QMouseEvent(*static_cast<QMouseEvent *>(e)));
    }
    default:
        break;
    }
    return false;
}

} // Render

} // Qt3DRender

QT_END_NAMESPACE
