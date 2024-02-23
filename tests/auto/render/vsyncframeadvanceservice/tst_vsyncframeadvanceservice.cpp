// Copyright (C) 2015 Paul Lemire
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QtTest/QTest>
#include <QSemaphore>
#include <QThread>
#include <Qt3DRender/private/vsyncframeadvanceservice_p.h>

class FakeRenderThread final : public QThread
{
public:
    FakeRenderThread(Qt3DRender::Render::VSyncFrameAdvanceService *tickService)
        : m_tickService(tickService)
        , m_running(1)
        , m_submitCount(0)
    {
    }

    int submitCount() const { return m_submitCount; }

    void stopRunning()
    {
        m_running.fetchAndStoreOrdered(0);
        m_submitSemaphore.release(1);
    }

    void enqueueRenderView()
    {
        m_submitSemaphore.release(1);
    }

protected:
    // QThread interface
    void run() final
    {
        m_tickService->proceedToNextFrame();

        while (true) {
            if (!isReadyToSubmit())
                break;
            ++m_submitCount;
            m_tickService->proceedToNextFrame();
        }
    }

private:
    bool isReadyToSubmit()
    {
        m_submitSemaphore.acquire(1);
        return m_running.loadRelaxed() == 1;
    }

    Qt3DRender::Render::VSyncFrameAdvanceService *m_tickService;
    QAtomicInt m_running;
    QSemaphore m_submitSemaphore;
    int m_submitCount;
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

        // WHEN
        renderThread.start();

        for (int i = 0; i < 10; ++i) {
            tickService.waitForNextFrame();
            renderThread.enqueueRenderView();
        }

        tickService.waitForNextFrame();

        renderThread.stopRunning();
        renderThread.wait();

        // THEN
        QCOMPARE(renderThread.submitCount(), 10);
    }
};

QTEST_MAIN(tst_VSyncFrameAdvanceService)

#include "tst_vsyncframeadvanceservice.moc"
