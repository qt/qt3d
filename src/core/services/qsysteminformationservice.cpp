/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
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

#include "qsysteminformationservice_p.h"
#include "qsysteminformationservice_p_p.h"

#ifdef Q_OS_ANDROID
#include <QtCore/QStandardPaths>
#endif

#include <QtCore/QThreadPool>
#include <QtCore/QCoreApplication>
#include <QtCore/QFile>
#include <QtCore/QDateTime>
#include <QtCore/QUrl>
#include <QtCore/QDir>
#include <QtGui/QDesktopServices>

#include <Qt3DCore/QAspectEngine>
#include <Qt3DCore/QAbstractAspect>
#include <Qt3DCore/private/qabstractaspect_p.h>
#include <Qt3DCore/private/qaspectengine_p.h>
#include <Qt3DCore/private/aspectcommanddebugger_p.h>

QT_BEGIN_NAMESPACE

namespace  {

struct FrameHeader
{
    FrameHeader()
        : frameId(0)
        , jobCount(0)
        , frameType(WorkerJob)
    {
    }

    enum FrameType {
        WorkerJob = 0,
        Submission
    };

    quint32 frameId;
    quint16 jobCount;
    quint16 frameType; // Submission or worker job
};

}
namespace Qt3DCore {

QSystemInformationServicePrivate::QSystemInformationServicePrivate(QAspectEngine *aspectEngine,
                                                                   const QString &description)
    : QAbstractServiceProviderPrivate(QServiceLocator::SystemInformation, description)
    , m_aspectEngine(aspectEngine)
    , m_submissionStorage(nullptr)
    , m_frameId(0)
    , m_commandDebugger(nullptr)
{
    m_traceEnabled = qEnvironmentVariableIsSet("QT3D_TRACE_ENABLED");
    m_graphicsTraceEnabled = qEnvironmentVariableIsSet("QT3D_GRAPHICS_TRACE_ENABLED");
    if (m_traceEnabled || m_graphicsTraceEnabled)
        m_jobsStatTimer.start();

    const bool commandServerEnabled = qEnvironmentVariableIsSet("QT3D_COMMAND_SERVER_ENABLED");
    if (commandServerEnabled) {
        m_commandDebugger = new Debug::AspectCommandDebugger(q_func());
        m_commandDebugger->initialize();
    }
}

QSystemInformationServicePrivate::~QSystemInformationServicePrivate() = default;

QSystemInformationServicePrivate *QSystemInformationServicePrivate::get(QSystemInformationService *q)
{
    return q->d_func();
}

// Called by the jobs
void QSystemInformationServicePrivate::addJobLogStatsEntry(QSystemInformationServicePrivate::JobRunStats &stats)
{
    if (!m_traceEnabled && !m_graphicsTraceEnabled)
        return;

    if (!m_jobStatsCached.hasLocalData()) {
        auto jobVector = new QVector<JobRunStats>;
        m_jobStatsCached.setLocalData(jobVector);
        QMutexLocker lock(&m_localStoragesMutex);
        m_localStorages.push_back(jobVector);
    }
    m_jobStatsCached.localData()->push_back(stats);
}

// Called from Submission thread (which can be main thread in Manual drive mode)
void QSystemInformationServicePrivate::addSubmissionLogStatsEntry(QSystemInformationServicePrivate::JobRunStats &stats)
{
    if (!m_traceEnabled && !m_graphicsTraceEnabled)
        return;

    QMutexLocker lock(&m_localStoragesMutex);
    if (!m_jobStatsCached.hasLocalData()) {
        m_submissionStorage = new QVector<JobRunStats>;
        m_jobStatsCached.setLocalData(m_submissionStorage);
    }

    // Handle the case where submission thread is also the main thread (Scene/Manual drive modes with no RenderThread)
    if (m_submissionStorage == nullptr && m_jobStatsCached.hasLocalData())
        m_submissionStorage = new QVector<JobRunStats>;

    // When having no submission thread this can be null
    m_submissionStorage->push_back(stats);
}

// Called after jobs have been executed (MainThread QAspectJobManager::enqueueJobs)
void QSystemInformationServicePrivate::writeFrameJobLogStats()
{
    if (!m_traceEnabled && !m_graphicsTraceEnabled)
        return;

    using JobRunStats = QSystemInformationServicePrivate::JobRunStats;

    if (!m_traceFile) {
        const QString fileName = QStringLiteral("trace_") + QCoreApplication::applicationName() +
                                 QDateTime::currentDateTime().toString(QStringLiteral("_yyMMdd-hhmmss_")) +
                                 QSysInfo::productType() + QStringLiteral("_") + QSysInfo::buildAbi() + QStringLiteral(".qt3d");
#ifdef Q_OS_ANDROID
        m_traceFile.reset(new QFile(QStandardPaths::writableLocation(QStandardPaths::DownloadLocation) + QStringLiteral("/") + fileName));
#else
        // TODO fix for iOS
        m_traceFile.reset(new QFile(fileName));
#endif
        if (!m_traceFile->open(QFile::WriteOnly|QFile::Truncate))
            qCritical("Failed to open trace file");
    }

    // Write Aspect + Job threads
    {
        FrameHeader header;
        header.frameId = m_frameId;
        header.jobCount = 0;

        for (const QVector<JobRunStats> *storage : qAsConst(m_localStorages))
            header.jobCount += storage->size();

        m_traceFile->write(reinterpret_cast<char *>(&header), sizeof(FrameHeader));

        for (QVector<JobRunStats> *storage : qAsConst(m_localStorages)) {
            for (const JobRunStats &stat : *storage)
                m_traceFile->write(reinterpret_cast<const char *>(&stat), sizeof(JobRunStats));
            storage->clear();
        }
    }

    // Write submission thread
    {
        QMutexLocker lock(&m_localStoragesMutex);
        const int submissionJobSize = m_submissionStorage != nullptr ? m_submissionStorage->size() : 0;
        if (submissionJobSize > 0) {
            FrameHeader header;
            header.frameId = m_frameId;
            header.jobCount = submissionJobSize;
            header.frameType = FrameHeader::Submission;

            m_traceFile->write(reinterpret_cast<char *>(&header), sizeof(FrameHeader));

            for (const JobRunStats &stat : *m_submissionStorage)
                m_traceFile->write(reinterpret_cast<const char *>(&stat), sizeof(JobRunStats));
            m_submissionStorage->clear();
        }
    }

    m_traceFile->flush();
    ++m_frameId;
}

void QSystemInformationServicePrivate::updateTracing()
{
    if (m_traceEnabled || m_graphicsTraceEnabled) {
        if (!m_jobsStatTimer.isValid())
            m_jobsStatTimer.start();
    } else {
        m_traceFile.reset();
    }
}


QTaskLogger::QTaskLogger(QSystemInformationService *service, const JobId &jobId, Type type)
    : m_service(service && service->isTraceEnabled() ? service : nullptr)
    , m_type(type)
{
    m_stats.jobId = jobId;
    if (m_service) {
        m_stats.startTime = QSystemInformationServicePrivate::get(m_service)->m_jobsStatTimer.nsecsElapsed();
        m_stats.threadId = reinterpret_cast<quint64>(QThread::currentThreadId());
    }
}

QTaskLogger::QTaskLogger(QSystemInformationService *service,
                         const quint32 jobType,
                         const quint32 instance,
                         QTaskLogger::Type type)
    : m_service(service && service->isTraceEnabled() ? service : nullptr)
    , m_type(type)
{
    m_stats.jobId.typeAndInstance[0] = jobType;
    m_stats.jobId.typeAndInstance[1] = instance;
    if (m_service) {
        m_stats.startTime = QSystemInformationServicePrivate::get(m_service)->m_jobsStatTimer.nsecsElapsed();
        m_stats.threadId = reinterpret_cast<quint64>(QThread::currentThreadId());
    }
}

QTaskLogger::~QTaskLogger() {
    if (m_service) {
        auto dservice = QSystemInformationServicePrivate::get(m_service);
        if (m_stats.endTime == 0L)
            m_stats.endTime = dservice->m_jobsStatTimer.nsecsElapsed();
        switch (m_type) {
        case AspectJob: dservice->addJobLogStatsEntry(m_stats); break;
        case Submission: dservice->addSubmissionLogStatsEntry(m_stats); break;
        }
    }
}

void QTaskLogger::end(qint64 t)
{
    m_stats.endTime = t > 0 || !m_service ? t : QSystemInformationServicePrivate::get(m_service)->m_jobsStatTimer.nsecsElapsed();
}

qint64 QTaskLogger::restart()
{
    if (m_service)
        m_stats.startTime = QSystemInformationServicePrivate::get(m_service)->m_jobsStatTimer.nsecsElapsed();
    return m_stats.startTime;
}


/* !\internal
    \class Qt3DCore::QSystemInformationService
    \inmodule Qt3DCore
    \brief Interface for a Qt3D system information service

    This is an interface class that should be subclassesd by providers of the
    system information service.
*/

/*
    Creates an instance of QSystemInformationService, with a \a description for
    the new service. This constructor is protected so only subclasses can
    instantiate a QSystemInformationService object.
*/

QSystemInformationService::QSystemInformationService(QAspectEngine *aspectEngine)
    : QAbstractServiceProvider(*new QSystemInformationServicePrivate(aspectEngine, QLatin1String("Default System Information Service")))
{
}

QSystemInformationService::QSystemInformationService(QAspectEngine *aspectEngine, const QString &description)
    : QAbstractServiceProvider(*new QSystemInformationServicePrivate(aspectEngine, description))
{
}

/*
    \internal
*/
QSystemInformationService::QSystemInformationService(QSystemInformationServicePrivate &dd)
    : QAbstractServiceProvider(dd)
{
}

bool QSystemInformationService::isTraceEnabled() const
{
    Q_D(const QSystemInformationService);
    return d->m_traceEnabled;
}

bool QSystemInformationService::isGraphicsTraceEnabled() const
{
    Q_D(const QSystemInformationService);
    return d->m_graphicsTraceEnabled;
}

bool QSystemInformationService::isCommandServerEnabled() const
{
    Q_D(const QSystemInformationService);
    return d->m_commandDebugger != nullptr;
}

void QSystemInformationService::setTraceEnabled(bool traceEnabled)
{
    Q_D(QSystemInformationService);
    if (d->m_traceEnabled != traceEnabled) {
        d->m_traceEnabled = traceEnabled;
        emit traceEnabledChanged(d->m_traceEnabled);
        d->updateTracing();
    }
}

void QSystemInformationService::setGraphicsTraceEnabled(bool graphicsTraceEnabled)
{
    Q_D(QSystemInformationService);
    if (d->m_graphicsTraceEnabled != graphicsTraceEnabled) {
        d->m_graphicsTraceEnabled = graphicsTraceEnabled;
        emit graphicsTraceEnabledChanged(d->m_graphicsTraceEnabled);
        d->updateTracing();
    }
}

/*
    \fn QStringList Qt3DCore::QSystemInformationService::aspectNames() const

    Returns a string list containing the names of all registered aspects.
*/
QStringList QSystemInformationService::aspectNames() const
{
    Q_D(const QSystemInformationService);
    if (!d->m_aspectEngine)
        return {};

    QStringList res;
    const auto aspects = d->m_aspectEngine->aspects();
    if (aspects.isEmpty())
        return { QLatin1String("No loaded aspects") };

    QAspectEnginePrivate *dengine = QAspectEnginePrivate::get(d->m_aspectEngine);
    for (auto aspect: aspects) {
        const QString name = dengine->m_factory.aspectName(aspect);
        if (!name.isEmpty())
            res << name;
        else
            res << QLatin1String("<unnamed>");
    }

    return  res;
}

/*
    \fn int Qt3DCore::QSystemInformationService::threadPoolThreadCount() const

    Returns the maximum number of threads in the Qt3D task manager's threadpool.
*/
int QSystemInformationService::threadPoolThreadCount() const
{
    return QThreadPool::globalInstance()->maxThreadCount();
}

void QSystemInformationService::writePreviousFrameTraces()
{
    Q_D(QSystemInformationService);
    d->writeFrameJobLogStats();
}

void QSystemInformationService::revealLogFolder()
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(QDir::currentPath()));
}

QVariant QSystemInformationService::executeCommand(const QString &command)
{
    Q_D(QSystemInformationService);

    if (command == QLatin1String("tracing on")) {
        setTraceEnabled(true);
        return  {isTraceEnabled()};
    }

    if (command == QLatin1String("tracing off")) {
        setTraceEnabled(false);
        return  {isTraceEnabled()};
    }

    if (command == QLatin1String("glprofiling on")) {
        setGraphicsTraceEnabled(true);
        return  {isTraceEnabled()};
    }

    if (command == QLatin1String("glprofiling off")) {
        setGraphicsTraceEnabled(false);
        return  {isTraceEnabled()};
    }

    return d->m_aspectEngine->executeCommand(command);
}

void QSystemInformationService::dumpCommand(const QString &command)
{
    QVariant res = executeCommand(command);
    QObject *obj = res.value<QObject *>();
    if (obj) {
        auto reply = qobject_cast<Qt3DCore::Debug::AsynchronousCommandReply*>(obj);
        if (reply) {
            connect(reply, &Debug::AsynchronousCommandReply::finished, this, [reply]() {
                qWarning() << qPrintable( QLatin1String(reply->data()) );
            });
            return;
        }
    }
    qWarning() << qPrintable(res.toString());
}

}

QT_END_NAMESPACE
