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

#include "qaspectjob.h"
#include "qaspectjob_p.h"
#include <QByteArray>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

namespace {

bool isDependencyNull(const QWeakPointer<QAspectJob> &dep)
{
    return dep.isNull();
}

} // anonymous

QAspectJobPrivate::QAspectJobPrivate()
{
}

QAspectJob::QAspectJob()
    : d_ptr(new QAspectJobPrivate)
{
}

/*! \internal */
QAspectJob::QAspectJob(QAspectJobPrivate &dd)
    : d_ptr(&dd)
{
}

QAspectJob::~QAspectJob()
{
    delete d_ptr;
}

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

QVector<QWeakPointer<QAspectJob> > QAspectJob::dependencies() const
{
    Q_D(const QAspectJob);
    return d->m_dependencies;
}

} // namespace Qt3DCore

QT_END_NAMESPACE
