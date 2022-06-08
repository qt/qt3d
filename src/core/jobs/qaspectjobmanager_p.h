// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DCORE_QASPECTJOBMANAGER_P_H
#define QT3DCORE_QASPECTJOBMANAGER_P_H

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

#include <Qt3DCore/qaspectjob.h>

#include <Qt3DCore/private/qabstractaspectjobmanager_p.h>
#include <Qt3DCore/private/qt3dcore_global_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

class QThreadPooler;
class DependencyHandler;
class QAspectManager;

class Q_3DCORE_PRIVATE_EXPORT QAspectJobManager : public QAbstractAspectJobManager
{
    Q_OBJECT
public:
    explicit QAspectJobManager(QAspectManager *parent = nullptr);
    ~QAspectJobManager();

    void initialize() override;

    void enqueueJobs(const std::vector<QAspectJobPtr> &jobQueue) override;

    int waitForAllJobs() override;

    void waitForPerThreadFunction(JobFunction func, void *arg) override;
    static int idealThreadCount();

private:
    QThreadPooler *m_threadPooler;
    QAspectManager *m_aspectManager;
};

} // namespace Qt3DCore

QT_END_NAMESPACE

#endif // QT3DCORE_QASPECTJOBMANAGER_P_H
