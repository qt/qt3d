/****************************************************************************
**
** Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL-EXCEPT$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtTest/QtTest>
#include <Qt3DCore/qaspectjob.h>
#include <Qt3DCore/qabstractaspect.h>
#include <Qt3DCore/private/qaspectmanager_p.h>
#include <Qt3DCore/private/qscheduler_p.h>
#include <private/qabstractaspect_p.h>
#include <private/qaspectjob_p.h>

using namespace Qt3DCore;

class JobPrivate : public QAspectJobPrivate
{
    bool m_postFrameCalled = false;


public:
    // QAspectJobPrivate interface
    void postFrame(QAspectManager *aspectManager)
    {
        Q_ASSERT(aspectManager);
        m_postFrameCalled = true;
    }

    bool postFrameCalled() const
    {
        return m_postFrameCalled;
    }
};

class Job : public QAspectJob
{
    bool m_wasExecuted = false;

public:
    Job()
        : QAspectJob(*new JobPrivate)
    {}

    bool wasExecuted() const
    {
        return m_wasExecuted;
    }

    bool postFrameCalled() const
    {
        Q_D(const Job);
        return d->postFrameCalled();
    }

    void run() override
    {
        m_wasExecuted = true;
    }

private:
    Q_DECLARE_PRIVATE(Job)
};
using JobPtr = QSharedPointer<Job>;

class AspectPrivate : public QAbstractAspectPrivate
{
    bool m_jobsDoneCalled = false;
    bool m_frameDoneCalled = false;

public:

    bool jobsDoneCalled() const
    {
        return m_jobsDoneCalled;
    }

    bool frameDoneCalled() const
    {
        return m_frameDoneCalled;
    }

    // QAspectJobProviderInterface interface
    void jobsDone() override
    {
        m_jobsDoneCalled = true;
    }

    void frameDone() override
    {
        m_frameDoneCalled = true;
    }
};

class Aspect : public QAbstractAspect
{
    Q_OBJECT

public:
    Aspect()
        : QAbstractAspect(*new AspectPrivate)
    {}

    JobPtr firstJob() const { return m_first; }
    JobPtr secondJob() const { return m_second; }

private:
    // QAbstractAspect interface
    QVector<QAspectJobPtr> jobsToExecute(qint64)
    {
        return { m_first, m_second };
    }

    JobPtr m_first = JobPtr::create();
    JobPtr m_second = JobPtr::create();
    Q_DECLARE_PRIVATE(Aspect)
};

class tst_QScheduler : public QObject
{
    Q_OBJECT
private Q_SLOTS:

    void checkInitialState()
    {
        // GIVEN
        QScheduler scheduler;

        // THEN
        QVERIFY(scheduler.aspectManager() == nullptr);

        // WHEN
        QAspectManager m;
        scheduler.setAspectManager(&m);

        // THEN
        QCOMPARE(scheduler.aspectManager(), &m);
    }

    void checkScheduleAndWaitForFrameAspectJobs()
    {
        // GIVEN
        QScheduler scheduler;
        QAspectManager manager;
        Aspect aspect;
        AspectPrivate *aspectPriv = static_cast<AspectPrivate *>(QObjectPrivate::get(&aspect));

        manager.registerAspect(&aspect);
        scheduler.setAspectManager(&manager);

        // THEN
        const JobPtr first = aspect.firstJob();
        const JobPtr second = aspect.secondJob();
        QVERIFY(!aspectPriv->jobsDoneCalled());
        QVERIFY(!aspectPriv->frameDoneCalled());
        QVERIFY(!first->wasExecuted());
        QVERIFY(!second->wasExecuted());
        QVERIFY(!first->postFrameCalled());
        QVERIFY(!second->postFrameCalled());

        // WHEN
        const int count = scheduler.scheduleAndWaitForFrameAspectJobs(0, false);

        // THEN
        QCOMPARE(count, 2);
        QVERIFY(first->wasExecuted());
        QVERIFY(second->wasExecuted());
        QVERIFY(first->postFrameCalled());
        QVERIFY(second->postFrameCalled());
        QVERIFY(aspectPriv->jobsDoneCalled());
        QVERIFY(!aspectPriv->frameDoneCalled());

        manager.unregisterAspect(&aspect);
    }
};

QTEST_MAIN(tst_QScheduler)

#include "tst_qscheduler.moc"
