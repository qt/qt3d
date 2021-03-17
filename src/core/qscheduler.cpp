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

#include "qscheduler_p.h"

#include <Qt3DCore/qabstractaspect.h>

#include <Qt3DCore/private/qabstractaspect_p.h>
#include <Qt3DCore/private/qaspectmanager_p.h>
#include <Qt3DCore/private/qaspectjob_p.h>
#include <Qt3DCore/private/qabstractaspectjobmanager_p.h>

#include <QtCore/QCoreApplication>
#include <QtCore/QDateTime>
#include <QtCore/QRegularExpression>

QT_BEGIN_NAMESPACE

namespace {

// Creates a graphviz dot file. To view online: https://dreampuf.github.io/GraphvizOnline/
void dumpJobs(QVector<Qt3DCore::QAspectJobPtr> jobs) {
    const QString fileName = QStringLiteral("qt3djobs_") + QCoreApplication::applicationName() +
        QDateTime::currentDateTime().toString(QStringLiteral("_yyMMdd-hhmmss")) + QStringLiteral(".dot");

    QFile f(fileName);
    if (!f.open(QFile::WriteOnly))
        return;

    auto formatJob = [](Qt3DCore::QAspectJob *job) -> QString {
        auto jobId = Qt3DCore::QAspectJobPrivate::get(job)->m_jobId;
        auto type = Qt3DCore::QAspectJobPrivate::get(job)->m_jobName.replace(QRegularExpression(QLatin1String("(^.*::)")), QLatin1String(""));
        return QString(QLatin1String("\"%1_%2\"")).arg(type).arg(jobId.typeAndInstance[1]);
    };

    QTextStream stream(&f);
    stream << "digraph qt3d_jobs {" << Qt::endl;

    for (const auto &job: jobs) {
        if (!Qt3DCore::QAspectJobPrivate::get(job.data())->isRequired())
            stream << QLatin1String("\t") << formatJob(job.data()) << QLatin1String(" [style=dotted]") << Qt::endl;
    }

    for (const auto &job: jobs) {
        auto dependencies = job->dependencies();
        for (const auto &dependency: dependencies)
            stream << QLatin1String("\t") << formatJob(dependency.toStrongRef().data()) << QLatin1String(" -> ") << formatJob(job.data()) << Qt::endl;
    }

    stream << "}" << Qt::endl;
}

}

namespace Qt3DCore {

QScheduler::QScheduler(QObject *parent)
    : QObject(parent)
    , m_aspectManager(nullptr)
{
}

QScheduler::~QScheduler()
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

int QScheduler::scheduleAndWaitForFrameAspectJobs(qint64 time, bool dumpJobs)
{
    QVector<QAspectJobPtr> jobQueue;

    // TODO: Allow clocks with custom scale factors and independent control
    //       over running / paused / stopped status
    // TODO: Advance all clocks registered with the engine

    // TODO: Set up dependencies between jobs as needed
    // For now just queue them up as they are
    const QVector<QAbstractAspect *> &aspects = m_aspectManager->aspects();
    for (QAbstractAspect *aspect : aspects) {
        const QVector<QAspectJobPtr> aspectJobs = QAbstractAspectPrivate::get(aspect)->jobsToExecute(time);
        jobQueue << aspectJobs;
    }

    if (dumpJobs)
        ::dumpJobs(jobQueue);

    m_aspectManager->jobManager()->enqueueJobs(jobQueue);

    // Do any other work here that the aspect thread can usefully be doing
    // whilst the threadpool works its way through the jobs

    int totalJobs = m_aspectManager->jobManager()->waitForAllJobs();

    {
        QTaskLogger logger(m_aspectManager->serviceLocator()->systemInformation(), 4097, 0, QTaskLogger::AspectJob);

        for (auto &job : qAsConst(jobQueue))
            QAspectJobPrivate::get(job.data())->postFrame(m_aspectManager);

        for (QAbstractAspect *aspect : aspects)
            QAbstractAspectPrivate::get(aspect)->jobsDone();
    }

    return totalJobs;
}

} // namespace Qt3DCore

QT_END_NAMESPACE
