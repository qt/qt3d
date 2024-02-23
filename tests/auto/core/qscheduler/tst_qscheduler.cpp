// Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QtTest/QtTest>
#include <Qt3DCore/qaspectjob.h>
#include <Qt3DCore/qabstractaspect.h>
#include <Qt3DCore/private/qaspectmanager_p.h>
#include <Qt3DCore/private/qaspectengine_p.h>
#include <Qt3DCore/private/qscheduler_p.h>
#include <private/qabstractaspect_p.h>
#include <private/qaspectjob_p.h>

using namespace Qt3DCore;

class JobPrivate : public QAspectJobPrivate
{
    bool m_postFrameCalled = false;


public:
    // QAspectJobPrivate interface
    void postFrame(QAspectManager *aspectManager) override
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
    std::vector<QAspectJobPtr> jobsToExecute(qint64) override
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
        Qt3DCore::QAspectEngine engine;
        auto manager = Qt3DCore::QAspectEnginePrivate::get(&engine)->m_aspectManager;
        QVERIFY(manager);
        manager->initialize();

        Aspect aspect;
        AspectPrivate *aspectPriv = static_cast<AspectPrivate *>(QObjectPrivate::get(&aspect));

        engine.registerAspect(&aspect);

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
        const int count = manager->scheduler()->scheduleAndWaitForFrameAspectJobs(0, false);

        // THEN
        QCOMPARE(count, 2);
        QVERIFY(first->wasExecuted());
        QVERIFY(second->wasExecuted());
        QVERIFY(first->postFrameCalled());
        QVERIFY(second->postFrameCalled());
        QVERIFY(aspectPriv->jobsDoneCalled());
        QVERIFY(!aspectPriv->frameDoneCalled());

        engine.unregisterAspect(&aspect);
    }
};

QTEST_MAIN(tst_QScheduler)

#include "tst_qscheduler.moc"
