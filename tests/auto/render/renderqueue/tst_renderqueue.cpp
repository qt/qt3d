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

#include <QtTest/QtTest>
#include <QMutex>
#include <QWaitCondition>
#include <QThread>
#include <Qt3DRenderer/private/renderqueue_p.h>
#include <Qt3DRenderer/private/renderview_p.h>

class tst_RenderQueue : public QObject
{
    Q_OBJECT
public :
    tst_RenderQueue() {}
    ~tst_RenderQueue() {}

private Q_SLOTS:
    void setRenderViewCount();
    void circleQueues();
    void checkOrder();
    void checkTimeToSubmit();
    void concurrentQueueAccess();
    void resetQueue();
};


void tst_RenderQueue::setRenderViewCount()
{
    // GIVEN
    Qt3D::Render::RenderQueue renderQueue;

    // WHEN
    renderQueue.setTargetRenderViewCount(7);

    // THEN
    QVERIFY(renderQueue.targetRenderViewCount() == 7);
    QVERIFY(renderQueue.currentRenderViewCount()== 0);
}

void tst_RenderQueue::circleQueues()
{
    // GIVEN
    Qt3D::Render::RenderQueue renderQueue;
    renderQueue.setTargetRenderViewCount(7);

    // WHEN
    for (int j = 0; j < 10; j++) {

        // WHEN
        renderQueue.reset();

        // THEN
        QVERIFY(!renderQueue.isFrameQueueComplete());
        QCOMPARE(renderQueue.currentRenderViewCount(), 0);

        // WHEN
        QList<Qt3D::Render::RenderView *> renderViews;
        for (int i = 0; i < 7; i++) {
            renderViews << new Qt3D::Render::RenderView();
            renderQueue.queueRenderView(renderViews.at(i), i);
        }

        // THEN
        QVERIFY(renderQueue.isFrameQueueComplete());
    }
}

void tst_RenderQueue::checkOrder()
{
    // GIVEN
    Qt3D::Render::RenderQueue renderQueue;
    renderQueue.setTargetRenderViewCount(7);
    QVector<Qt3D::Render::RenderView *> renderViews(7);

    // WHEN
    for (int i = 0; i < 7; ++i) {
        int processingOrder = (i % 2 == 0) ? (6 - i) : i;
        renderViews[processingOrder] = new Qt3D::Render::RenderView();
        renderQueue.queueRenderView(renderViews[processingOrder], processingOrder);
    }

    // THEN
    QVector<Qt3D::Render::RenderView *> frame = renderQueue.nextFrameQueue();
    for (int i = 0; i < 7; ++i) {
        QVERIFY(frame[i] == renderViews[i]);
    }
}

void tst_RenderQueue::checkTimeToSubmit()
{
    // GIVEN
    Qt3D::Render::RenderQueue renderQueue;
    renderQueue.setTargetRenderViewCount(7);
    QVector<Qt3D::Render::RenderView *> renderViews(7);

    // WHEN
    for (int i = 0; i < 7; i++) {
        int processingOrder = (i % 2 == 0) ? (6 - i) : i;
        renderViews[processingOrder] = new Qt3D::Render::RenderView();
        renderQueue.queueRenderView(renderViews[processingOrder], processingOrder);

        // THEN
        if (i < 6)
            QVERIFY(!renderQueue.isFrameQueueComplete());
        else
            QVERIFY(renderQueue.isFrameQueueComplete());
    }
}

class SimpleWorker : public QThread
{
    Q_OBJECT
public:
    QWaitCondition m_waitTimeToSubmit;
    QWaitCondition m_waitToFillQueue;
    QMutex m_mutex;
    Qt3D::Render::RenderQueue *m_renderQueues;

public Q_SLOTS:

    void run() Q_DECL_FINAL // In Thread
    {
        for (int i = 0; i < 5; i++) {
            QMutexLocker lock(&m_mutex);
            m_waitToFillQueue.wait(&m_mutex);
            lock.unlock();

            QVERIFY(m_renderQueues->currentRenderViewCount() == 0);
            QVERIFY(!m_renderQueues->isFrameQueueComplete());

            for (int j = 0; j < 7; ++j) {
                // THEN
                QCOMPARE(m_renderQueues->currentRenderViewCount(), j);
                // WHEN
                m_renderQueues->queueRenderView(new Qt3D::Render::RenderView(), j);
                // THEN
                QVERIFY(m_renderQueues->targetRenderViewCount() == 7);
                QCOMPARE(m_renderQueues->currentRenderViewCount(), j + 1);
                QVERIFY(m_renderQueues->isFrameQueueComplete() == (j == 6));
                QThread::msleep(20); // Simulates business
            }

            QVERIFY(m_renderQueues->isFrameQueueComplete());
            m_waitTimeToSubmit.wakeOne();
        }
    }
};


void tst_RenderQueue::concurrentQueueAccess()
{
    // GIVEN
    Qt3D::Render::RenderQueue *renderQueue = new Qt3D::Render::RenderQueue;

    SimpleWorker *jobsThread = new SimpleWorker();
    renderQueue->setTargetRenderViewCount(7);
    jobsThread->m_renderQueues = renderQueue;

    // THEN
    QVERIFY(jobsThread->m_renderQueues->targetRenderViewCount() == renderQueue->targetRenderViewCount());
    QVERIFY(jobsThread->m_renderQueues->currentRenderViewCount() == renderQueue->currentRenderViewCount());


    // Start thread
    jobsThread->start();

    QThread::msleep(500); // To be sure the thread is properly started

    jobsThread->m_waitToFillQueue.wakeAll();

    for (int i = 0; i < 5; ++i) {
        QMutexLocker lock(&jobsThread->m_mutex);
        jobsThread->m_waitTimeToSubmit.wait(&jobsThread->m_mutex);
        lock.unlock();

        // WHEN unlocked
        // THEN
        QVERIFY (renderQueue->isFrameQueueComplete());
        QCOMPARE(renderQueue->nextFrameQueue().size(), renderQueue->targetRenderViewCount());

        // reset queue for next frame
        renderQueue->reset();
        jobsThread->m_waitToFillQueue.wakeAll();
    }
    jobsThread->wait();
}

void tst_RenderQueue::resetQueue()
{
    // GIVEN
    Qt3D::Render::RenderQueue renderQueue;

    for (int j = 0; j < 5; j++) {
        // WHEN
        renderQueue.setTargetRenderViewCount(5);
        // THEN
        QVERIFY(renderQueue.currentRenderViewCount() == 0);

        // WHEN
        QVector<Qt3D::Render::RenderView *> renderViews(5);
        for (int i = 0; i < 5; ++i) {
            renderQueue.queueRenderView(renderViews.at(i), i);
        }
        // THEN
        QCOMPARE(renderQueue.currentRenderViewCount(), 5);
        QVERIFY(renderQueue.isFrameQueueComplete());

        // WHEN
        renderQueue.reset();
        // THEN
        QVERIFY(renderQueue.currentRenderViewCount() == 0);
    }
}

QTEST_APPLESS_MAIN(tst_RenderQueue)

#include "tst_renderqueue.moc"
