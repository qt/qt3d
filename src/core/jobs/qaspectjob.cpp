/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
****************************************************************************/

#include "qaspectjob.h"
#include "qaspectjob_p.h"

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
    Q_UNUSED(aspectManager)
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
 * Executes the job.
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
    d->m_dependencies.append(dependency);
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
        d->m_dependencies.removeAll(dependency);
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
QVector<QWeakPointer<QAspectJob> > QAspectJob::dependencies() const
{
    Q_D(const QAspectJob);
    return d->m_dependencies;
}

void QAspectJob::postFrame(QAspectManager *aspectManager)
{
    Q_D(QAspectJob);
    if (aspectManager)
        d->postFrame(aspectManager);
}

} // namespace Qt3DCore

QT_END_NAMESPACE
