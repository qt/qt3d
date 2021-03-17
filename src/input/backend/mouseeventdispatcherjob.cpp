/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "mouseeventdispatcherjob_p.h"
#include <Qt3DCore/private/qaspectmanager_p.h>
#include <Qt3DInput/qmousehandler.h>
#include <Qt3DInput/private/qmousehandler_p.h>
#include <Qt3DInput/private/inputhandler_p.h>
#include <Qt3DInput/private/inputmanagers_p.h>
#include <Qt3DInput/private/job_common_p.h>
#include <Qt3DInput/private/mousehandler_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {
namespace Input {

class MouseEventDispatcherJobPrivate : public Qt3DCore::QAspectJobPrivate
{
public:
    MouseEventDispatcherJobPrivate() { }
    ~MouseEventDispatcherJobPrivate() override { }

    void postFrame(Qt3DCore::QAspectManager *manager) override;

    Qt3DCore::QNodeId m_mouseInput;
    QList<QT_PREPEND_NAMESPACE(QMouseEvent)> m_mouseEvents;
#if QT_CONFIG(wheelevent)
    QList<QT_PREPEND_NAMESPACE(QWheelEvent)> m_wheelEvents;
#endif
};

MouseEventDispatcherJob::MouseEventDispatcherJob(Qt3DCore::QNodeId input,
                                                 const QList<QT_PREPEND_NAMESPACE (QMouseEvent)> &mouseEvents
#if QT_CONFIG(wheelevent)
                                               , const QList<QT_PREPEND_NAMESPACE (QWheelEvent)> &wheelEvents
#endif
                                                                                                             )
    : QAspectJob(*new MouseEventDispatcherJobPrivate)
    , m_inputHandler(nullptr)
{
    Q_D(MouseEventDispatcherJob);
    d->m_mouseInput = input;
    d->m_mouseEvents = mouseEvents;
#if QT_CONFIG(wheelevent)
    d->m_wheelEvents = wheelEvents;
#endif
    SET_JOB_RUN_STAT_TYPE(this, JobTypes::MouseEventDispatcher, 0)
}

void MouseEventDispatcherJob::setInputHandler(InputHandler *handler)
{
    m_inputHandler = handler;
}

void MouseEventDispatcherJob::run()
{
    // NOP
}

void MouseEventDispatcherJobPrivate::postFrame(Qt3DCore::QAspectManager *manager)
{
    QMouseHandler *node = qobject_cast<QMouseHandler *>(manager->lookupNode(m_mouseInput));
    if (!node)
        return;

    QMouseHandlerPrivate *dnode = static_cast<QMouseHandlerPrivate *>(QMouseHandlerPrivate::get(node));

    for (const QT_PREPEND_NAMESPACE(QMouseEvent) &e : m_mouseEvents)
        dnode->mouseEvent(QMouseEventPtr::create(e));
#if QT_CONFIG(wheelevent)
    for (const QT_PREPEND_NAMESPACE(QWheelEvent) &e : m_wheelEvents) {
        QWheelEvent we(e);
        emit node->wheel(&we);
    }
#endif
}

} // namespace Input
} // namespace Qt3DInput

QT_END_NAMESPACE
