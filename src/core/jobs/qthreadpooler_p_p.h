/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
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

#ifndef QT3D_QTHREADPOOLER_P_H
#define QT3D_QTHREADPOOLER_P_H

#include "jobrunner_p.h"
#include "dependencyhandler_p.h"

#include <QtCore/QtGlobal>
#include <private/qobject_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

class QThreadPooler;

class QThreadPoolerPrivate : public QObjectPrivate
{
public:
    QThreadPoolerPrivate(QThreadPooler *qq);
    ~QThreadPoolerPrivate();

    bool isQueueEmpty();

    void incRunningThreads();
    void decRunningThreads();

    inline void setDependencyHandler(DependencyHandler *handler)
    {
        m_dependencyHandler = handler;
    }

    void createRunners(int threadCount);
    void shutdown();

    int maxThreadCount() const;
    void setMaxThreadCount(int threadCount);

    Q_DECLARE_PUBLIC(QThreadPooler)

private:
    QList<JobRunner *> m_workers;
    QVector<QSharedPointer<TaskInterface> > m_taskQueue;

    QWaitCondition m_jobAvailable;
    QWaitCondition m_jobFinished;
    QMutex *m_mutex;
    int m_runningThreads;
    int m_maxThreadCount;
    DependencyHandler *m_dependencyHandler;
};

}

QT_END_NAMESPACE

#endif // QT3D_QTHREADPOOLER_P_H

