// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DCORE_QSYSTEMINFORMATIONSERVICE_P_P_H
#define QT3DCORE_QSYSTEMINFORMATIONSERVICE_P_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtCore/QThreadStorage>
#include <QtCore/QElapsedTimer>
#include <QtCore/QFile>
#include <QtCore/QMutex>

#include <Qt3DCore/qt3dcore_global.h>
#include <Qt3DCore/private/qt3dcore_global_p.h>
#include <Qt3DCore/private/qabstractserviceprovider_p.h>
#include <Qt3DCore/private/qservicelocator_p.h>
#include <Qt3DCore/private/qsysteminformationservice_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

namespace Debug {
class AspectCommandDebugger;
} // Debug

union Q_3DCORE_PRIVATE_EXPORT JobId
{
    JobId() : id(0L) { }
    JobId(quint32 t, quint32 i) { typeAndInstance[0] = t; typeAndInstance[1] = i; }

    quint32 typeAndInstance[2];
    quint64 id;
};

class Q_3DCORE_PRIVATE_EXPORT QSystemInformationServicePrivate : public QAbstractServiceProviderPrivate
{
public:
    struct JobRunStats
    {
        JobRunStats() { jobId.id = 0; startTime = 0L; endTime = 0L; }

        qint64 startTime;
        qint64 endTime;
        JobId jobId; // QAspectJob subclasses should properly populate the jobId
        quint64 threadId;
    };

    QSystemInformationServicePrivate(QAspectEngine *aspectEngine, const QString &description);
    ~QSystemInformationServicePrivate();

    static QSystemInformationServicePrivate *get(QSystemInformationService *q);

    // Aspects + Job threads
    void addJobLogStatsEntry(JobRunStats &stats);

    // Submission thread
    void addSubmissionLogStatsEntry(JobRunStats &stats);

    void writeFrameJobLogStats();
    void updateTracing();

    QAspectEngine *m_aspectEngine;
    bool m_traceEnabled;
    bool m_graphicsTraceEnabled;

    QElapsedTimer m_jobsStatTimer;
    QThreadStorage<QList<JobRunStats> *> m_jobStatsCached;

    QList<QList<JobRunStats> *> m_localStorages;
    QList<JobRunStats> *m_submissionStorage;

    QMutex m_localStoragesMutex;

    QScopedPointer<QFile> m_traceFile;
    quint32 m_frameId;

    Debug::AspectCommandDebugger *m_commandDebugger;

    Q_DECLARE_PUBLIC(QSystemInformationService)
};

class Q_3DCORE_PRIVATE_EXPORT QTaskLogger {
public:
    enum Type {
        AspectJob,
        Submission
    };

    QTaskLogger(QSystemInformationService *service, const JobId &jobId, Type type);
    QTaskLogger(QSystemInformationService *service, const quint32 jobType, const quint32 instance, Type type = Submission);

    ~QTaskLogger();

    void end(qint64 t = 0L);
    qint64 restart();

private:
    QSystemInformationService *m_service;
    QSystemInformationServicePrivate::JobRunStats m_stats;
    Type m_type;
};

}

QT_END_NAMESPACE

#endif // QT3DCORE_QSYSTEMINFORMATIONSERVICE_P_P_H

