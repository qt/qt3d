/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
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

#include "jobrunner_p.h"
#include "qthreadpooler_p.h"

#include <QtCore/QThread>
#include <QtCore/QMutexLocker>
#include <QtCore/QAtomicInt>

QT_BEGIN_NAMESPACE

namespace Qt3D {

JobRunner::JobRunner(QThreadPooler *parent)
    : QThread(parent),
      m_abort(0),
      m_pooler(parent),
      m_jobAvailable(Q_NULLPTR),
      m_mutex(Q_NULLPTR)
{
    QObject::connect(parent, SIGNAL(shuttingDown()), this, SLOT(shutDown()));
}

JobRunner::~JobRunner()
{
    shutDown();
}

void JobRunner::run()
{
    Q_ASSERT(m_jobAvailable != Q_NULLPTR);

    while (!m_abort.load()) {
        if (const QSharedPointer<TaskInterface> task = m_pooler->nextTask()) {
            m_pooler->startRunning();
            task->run(task, this);
            m_pooler->stopRunning();
        } else {
            suspend();
        }
    }
}

void JobRunner::suspend()
{
    const QMutexLocker locker(m_mutex);

    m_jobAvailable->wait(m_mutex);
}

void JobRunner::shutDown()
{
    m_abort.store(1);
}

} // namespace Qt3D

QT_END_NAMESPACE
