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

#include "keyeventdispatcherjob_p.h"
#include <Qt3DCore/private/qaspectmanager_p.h>
#include <Qt3DInput/qkeyboardhandler.h>
#include <Qt3DInput/private/qkeyboardhandler_p.h>
#include <Qt3DInput/private/inputhandler_p.h>
#include <Qt3DInput/private/inputmanagers_p.h>
#include <Qt3DInput/private/job_common_p.h>
#include <Qt3DInput/private/keyboardhandler_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {
namespace Input {

class KeyEventDispatcherJobPrivate : public Qt3DCore::QAspectJobPrivate
{
public:
    KeyEventDispatcherJobPrivate() { }
    ~KeyEventDispatcherJobPrivate() override { }

    void postFrame(Qt3DCore::QAspectManager *manager) override;

    Qt3DCore::QNodeId m_keyboardHandler;
    QList<QT_PREPEND_NAMESPACE(QKeyEvent)> m_events;
};

KeyEventDispatcherJob::KeyEventDispatcherJob(Qt3DCore::QNodeId input, const QList<QT_PREPEND_NAMESPACE(QKeyEvent)> &events)
    : QAspectJob(*new KeyEventDispatcherJobPrivate)
    , m_inputHandler(nullptr)
{
    Q_D(KeyEventDispatcherJob);
    d->m_keyboardHandler = input;
    d->m_events = events;
    SET_JOB_RUN_STAT_TYPE(this, JobTypes::KeyEventDispatcher, 0)
}

void KeyEventDispatcherJob::setInputHandler(InputHandler *handler)
{
    m_inputHandler = handler;
}

void KeyEventDispatcherJob::run()
{
    // NOP
}

void KeyEventDispatcherJobPrivate::postFrame(Qt3DCore::QAspectManager *manager)
{
    QKeyboardHandler *node = qobject_cast<QKeyboardHandler *>(manager->lookupNode(m_keyboardHandler));
    if (!node)
        return;

    QKeyboardHandlerPrivate *dnode = static_cast<QKeyboardHandlerPrivate *>(QKeyboardHandlerPrivate::get(node));
    for (const auto &e: qAsConst(m_events)) {
        QKeyEvent ke(e);
        dnode->keyEvent(&ke);
    }

    m_events.clear();
}

} // namespace Input
} // namespace Qt3DInput

QT_END_NAMESPACE
