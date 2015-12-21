/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
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

#include "qscheduler_p.h"

#include "qabstractaspect.h"
#include "qabstractaspect_p.h"
#include "qaspectmanager_p.h"
#include "qabstractaspectjobmanager_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

QScheduler::QScheduler(QObject *parent)
    : QObject(parent)
{
}

void QScheduler::setAspectManager(QAspectManager *aspectManager)
{
    m_aspectManager = aspectManager;
}

QAspectManager *QScheduler::aspectManager() const
{
    return m_aspectManager;
}

void QScheduler::scheduleAndWaitForFrameAspectJobs(qint64 time)
{
    QVector<QAspectJobPtr> jobQueue;

    // TODO: Allow clocks with custom scale factors and independent control
    //       over running / paused / stopped status
    // TODO: Advance all clocks registered with the engine

    // TODO: Set up dependencies between jobs as needed
    // For now just queue them up as they are
    const QList<QAbstractAspect *> &aspects = m_aspectManager->aspects();
    Q_FOREACH (QAbstractAspect *aspect, aspects) {
        QVector<QAspectJobPtr> aspectJobs = QAbstractAspectPrivate::get(aspect)->jobsToExecute(time);
        jobQueue << aspectJobs;
    }

    m_aspectManager->jobManager()->enqueueJobs(jobQueue);

    // Do any other work here that the aspect thread can usefully be doing
    // whilst the threadpool works its way through the jobs

    m_aspectManager->jobManager()->waitForAllJobs();
}

} // namespace Qt3DCore

QT_END_NAMESPACE
