/****************************************************************************
**
** Copyright (C) 2015 Paul Lemire
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

#include <Qt3DRender/private/vsyncframeadvanceservice_p.h>

class FakeRenderThread Q_DECL_FINAL : public QThread
{
public:
    FakeRenderThread(Qt3DRender::Render::VSyncFrameAdvanceService *tickService)
        : m_tickService(tickService)
    {
    }

    // QThread interface
protected:
    void run() Q_DECL_FINAL
    {
        QThread::msleep(1000);
        m_tickService->proceedToNextFrame();
    }

private:
    Qt3DRender::Render::VSyncFrameAdvanceService *m_tickService;
};

class FakeAspectThread Q_DECL_FINAL : public QThread
{
public:
    FakeAspectThread(Qt3DRender::Render::VSyncFrameAdvanceService *tickService)
        : m_tickService(tickService)
        , m_count(0)
        , m_running(true)
        , m_waitForStarted(0)
    {
    }

    int count() const { return m_count; }

    void stopRunning()
    {
        QMutexLocker lock(&m_mutex);
        m_running = false;
    }

    void waitForStarted()
    {
        m_waitForStarted.acquire(1);
    }

protected:
    // QThread interface
    void run() Q_DECL_FINAL
    {
        m_waitForStarted.release(1);
        while (true) {

            bool running = true;
            {
                QMutexLocker lock(&m_mutex);
                running = m_running;
            }

            if (!running) {
                qDebug() << "exiting";
                return;
            }

            m_tickService->waitForNextFrame();
            ++m_count;

            QThread::msleep(100);
        }
    }

private:
    Qt3DRender::Render::VSyncFrameAdvanceService *m_tickService;
    int m_count;
    bool m_running;
    QMutex m_mutex;
    QSemaphore m_waitForStarted;
};

class tst_VSyncFrameAdvanceService : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void checkSynchronisation()
    {
        // GIVEN
        Qt3DRender::Render::VSyncFrameAdvanceService tickService(true);
        FakeRenderThread renderThread(&tickService);
        QElapsedTimer t;

        // WHEN
        t.start();
        renderThread.start();
        tickService.waitForNextFrame();

        // THEN
        // we allow for a little margin by checking for 950
        // instead of 1000
        QVERIFY(t.elapsed() >= 950);
    }

    void checkWaitForNextFrame()
    {
        // GIVEN
        Qt3DRender::Render::VSyncFrameAdvanceService tickService(false);
        FakeAspectThread aspectThread(&tickService);

        // WHEN
        aspectThread.start();
        aspectThread.waitForStarted();

        QElapsedTimer t;
        t.start();

        while (t.elapsed() < 1000)
            tickService.proceedToNextFrame();

        aspectThread.stopRunning();

        // To make sure the aspectThread can finish
        tickService.proceedToNextFrame();
        aspectThread.wait();

        // THEN
        QCOMPARE(aspectThread.count(), 10);
    }
};

QTEST_MAIN(tst_VSyncFrameAdvanceService)

#include "tst_vsyncframeadvanceservice.moc"
