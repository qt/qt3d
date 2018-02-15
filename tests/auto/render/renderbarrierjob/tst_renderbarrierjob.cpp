/****************************************************************************
**
** Copyright (C) 2018 The Qt Company Ltd.
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
#include <QMutex>
#include <QWaitCondition>
#include <QThread>
#include <Qt3DRender/private/renderbarrierjob_p.h>
#include <Qt3DRender/private/job_common_p.h>

class TestRenderThread : public QThread
{
public:
    TestRenderThread(Qt3DRender::Render::RenderBarrierJobPtr barrier)
        : m_barrier(barrier)
    {}

    void run() override
    {
        m_started.store(1);
        m_writeSemaphore.release();
        m_readSemaphore.acquire();

        m_barrier->allowToProceed();
        m_released.store(1);
        m_writeSemaphore.release();
        m_readSemaphore.acquire();

        m_barrier->waitForDependencies();
        m_completed.store(1);
    }

    Qt3DRender::Render::RenderBarrierJobPtr m_barrier;
    QSemaphore m_writeSemaphore;
    QSemaphore m_readSemaphore;

    QAtomicInt m_started;
    QAtomicInt m_released;
    QAtomicInt m_completed;
};

class tst_RenderBarrierJob : public QObject
{
    Q_OBJECT
public :
    tst_RenderBarrierJob() {}
    ~tst_RenderBarrierJob() {}

private Q_SLOTS:
    void shouldControlRenderThread()
    {
        // GIVEN a barrier (of any type)
        auto barrier = Qt3DRender::Render::RenderBarrierJobPtr::create(Qt3DRender::Render::JobTypes::BeginDrawingBarrier);
        TestRenderThread testThread(barrier);

        // THEN
        QVERIFY(testThread.m_started.load() == 0);
        QVERIFY(testThread.m_released.load() == 0);
        QVERIFY(testThread.m_completed.load() == 0);

        // WHEN
        testThread.start();
        testThread.m_writeSemaphore.acquire();

        // THEN thread should have started
        QVERIFY(testThread.m_started.load() == 1);
        QVERIFY(testThread.m_released.load() == 0);
        QVERIFY(testThread.m_completed.load() == 0);
        testThread.m_readSemaphore.release();

        // WHEN
        testThread.m_writeSemaphore.acquire();

        // THEN job should be released
        QVERIFY(testThread.m_started.load() == 1);
        QVERIFY(testThread.m_released.load() == 1);
        QVERIFY(testThread.m_completed.load() == 0);
        testThread.m_readSemaphore.release();

        // WHEN job runs and we wait for thread
        barrier->run();
        testThread.wait();

        // THEN thread should be finished
        QVERIFY(testThread.m_started.load() == 1);
        QVERIFY(testThread.m_released.load() == 1);
        QVERIFY(testThread.m_completed.load() == 1);
    }
};

QTEST_MAIN(tst_RenderBarrierJob)

#include "tst_renderbarrierjob.moc"
