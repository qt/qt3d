/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qscheduler.h"

#include "qabstractaspect.h"
#include "qaspectmanager.h"
#include "qjobmanagerinterface.h"

QT_BEGIN_NAMESPACE

namespace Qt3D {

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

void QScheduler::update(qint64 time)
{
    Q_UNUSED(time);

    // Get tasks for this frame from each aspect
    const QList<QAbstractAspect *> &aspects = m_aspectManager->aspects();
    QHash<QAbstractAspect::AspectType, QVector<QJobPtr> > jobs;
    Q_FOREACH (QAbstractAspect *aspect, aspects) {
        QVector<QJobPtr> aspectJobs = aspect->jobsToExecute();
        jobs.insert(aspect->aspectType(), aspectJobs);
    }

    // TODO: Set up dependencies between jobs as needed
    // For now just queue them up as they are with render tasks first
    QVector<QJobPtr> jobQueue;
    for (int i = QAbstractAspect::AspectRenderer; i <= QAbstractAspect::AspectOther; ++i) {
        QAbstractAspect::AspectType aspectType = static_cast<QAbstractAspect::AspectType>(i);
        if (jobs.contains(aspectType))
            jobQueue += jobs.value(aspectType);
    }

    m_aspectManager->jobManager()->enqueueJobs(jobQueue);

    // Do any other work here that the aspect thread can usefully be doing
    // whilst the threadpool works its way through the jobs

    m_aspectManager->jobManager()->waitForAllJobs();
}

} // namespace Qt3D

QT_END_NAMESPACE
