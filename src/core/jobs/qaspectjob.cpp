// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qaspectjob.h"
#include "qaspectjob_p.h"

#include <Qt3DCore/qaspectengine.h>
#include <Qt3DCore/private/qaspectengine_p.h>

#include <QtCore/QByteArray>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

namespace {

bool isDependencyNull(const QWeakPointer<QAspectJob> &dep)
{
    return dep.isNull();
}

} // anonymous

QAspectJobPrivate::QAspectJobPrivate()
    : m_jobName(QLatin1String("UnknowJob"))
{
}

QAspectJobPrivate::~QAspectJobPrivate() = default;

QAspectJobPrivate *QAspectJobPrivate::get(QAspectJob *job)
{
    return job->d_func();
}

bool QAspectJobPrivate::isRequired() const
{
    return true;
}

void QAspectJobPrivate::postFrame(QAspectManager *aspectManager)
{
    Q_UNUSED(aspectManager);
}

QAspectJob::QAspectJob()
    : d_ptr(new QAspectJobPrivate)
{
}

/*!
 * \class Qt3DCore::QAspectJob
 * \inheaderfile Qt3DCore/QAspectJob
 * \inmodule Qt3DCore
 * \brief The base class for jobs executed in an aspect.
 */

/*!
 * \fn void Qt3DCore::QAspectJob::run()
 * Executes the job. This is called on a separate thread by the scheduler.
 */

/*!
 * \internal
 */
QAspectJob::QAspectJob(QAspectJobPrivate &dd)
    : d_ptr(&dd)
{
}

QAspectJob::~QAspectJob()
{
    delete d_ptr;
}

/*!
 * Adds \a dependency to the aspect job.
 */
void QAspectJob::addDependency(QWeakPointer<QAspectJob> dependency)
{
    Q_D(QAspectJob);
    d->m_dependencies.push_back(dependency);
#ifdef QT3DCORE_ASPECT_JOB_DEBUG
    static int threshold = qMax(1, qgetenv("QT3DCORE_ASPECT_JOB_DEPENDENCY_THRESHOLD").toInt());
    if (d->m_dependencies.count() > threshold)
        qWarning() << "Suspicious number of job dependencies found";
#endif
}

/*!
 * Removes the given \a dependency from aspect job.
 */
void QAspectJob::removeDependency(QWeakPointer<QAspectJob> dependency)
{
    Q_D(QAspectJob);
    if (!dependency.isNull()) {
        d->m_dependencies.erase(std::remove(d->m_dependencies.begin(),
                                            d->m_dependencies.end(),
                                            dependency),
                                d->m_dependencies.end());
    } else {
        d->m_dependencies.erase(std::remove_if(d->m_dependencies.begin(),
                                               d->m_dependencies.end(),
                                               isDependencyNull),
                                d->m_dependencies.end());
    }
}

/*!
 * \return the dependencies of the aspect job.
 */
const std::vector<QWeakPointer<QAspectJob> > &QAspectJob::dependencies() const
{
    Q_D(const QAspectJob);
    return d->m_dependencies;
}

/*!
 * Called in the main thread when all the jobs have completed.
 * This is a good point to push changes back to the frontend.
 * \a aspectEngine is the engine responsible for the run loop.
 */
void QAspectJob::postFrame(QAspectEngine *aspectEngine)
{
    Q_D(QAspectJob);
    if (aspectEngine) {
        auto manager = QAspectEnginePrivate::get(aspectEngine)->m_aspectManager;
        d->postFrame(manager);
    }
}

/*!
 * Should return true (default) if the job has actually something to do.
 * If returning false, the job will not be scheduled (but it's dependencies
 * will be).
 */
bool QAspectJob::isRequired()
{
    Q_D(QAspectJob);
    return d->isRequired();
}

} // namespace Qt3DCore

QT_END_NAMESPACE
