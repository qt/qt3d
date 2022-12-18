// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qscheduler_p.h"

#include <Qt3DCore/qabstractaspect.h>

#include <Qt3DCore/private/qabstractaspect_p.h>
#include <Qt3DCore/private/qaspectmanager_p.h>
#include <Qt3DCore/private/qaspectjob_p.h>
#include <Qt3DCore/private/qabstractaspectjobmanager_p.h>
#include <Qt3DCore/private/vector_helper_p.h>

#include <QtCore/QCoreApplication>
#include <QtCore/QDateTime>
#include <QtCore/QRegularExpression>

QT_BEGIN_NAMESPACE

namespace {

// Creates a graphviz dot file. To view online: https://dreampuf.github.io/GraphvizOnline/
void dumpJobs(const std::vector<Qt3DCore::QAspectJobPtr> &jobs) {
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
    std::vector<QAspectJobPtr> jobQueue;

    // TODO: Allow clocks with custom scale factors and independent control
    //       over running / paused / stopped status
    // TODO: Advance all clocks registered with the engine

    // TODO: Set up dependencies between jobs as needed
    // For now just queue them up as they are
    const QList<QAbstractAspect *> &aspects = m_aspectManager->aspects();
    for (QAbstractAspect *aspect : aspects) {
        std::vector<QAspectJobPtr> aspectJobs = QAbstractAspectPrivate::get(aspect)->jobsToExecute(time);
        Qt3DCore::moveAtEnd(jobQueue, std::move(aspectJobs));
    }

    if (jobQueue.empty())
        return 0;

    if (dumpJobs)
        ::dumpJobs(jobQueue);

    m_aspectManager->jobManager()->enqueueJobs(jobQueue);

    // Do any other work here that the aspect thread can usefully be doing
    // whilst the threadpool works its way through the jobs

    const int totalJobs = m_aspectManager->jobManager()->waitForAllJobs();

    {
        QTaskLogger logger(m_aspectManager->serviceLocator()->systemInformation(), 4097, 0, QTaskLogger::AspectJob);

        for (auto &job : std::as_const(jobQueue))
            job->postFrame(m_aspectManager->engine());

        for (QAbstractAspect *aspect : aspects)
            aspect->jobsDone();
    }

    return totalJobs;
}

} // namespace Qt3DCore

QT_END_NAMESPACE

#include "moc_qscheduler_p.cpp"
