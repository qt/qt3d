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
#include <Qt3DRenderer/private/renderqueues_p.h>
#include <Qt3DRenderer/private/renderview_p.h>

class tst_RenderQueues : public QObject
{
    Q_OBJECT
public :
    tst_RenderQueues() {}
    ~tst_RenderQueues() {}

private slots:
    void initializeQueue();
    void setRenderViewCount();
    void circleQueues();
    void checkOrder();
    void checkTimeToSubmit();
    void submitRenderViews();
    void concurrentQueueAccess();
    void resetQueues();

};

void tst_RenderQueues::initializeQueue()
{
    Qt3D::Render::RenderQueues renderQueues(15);
    QVERIFY(renderQueues.queuedFrames() == 0);
    QVERIFY(renderQueues.framesCapacity() == 15);
}

void tst_RenderQueues::setRenderViewCount()
{
    Qt3D::Render::RenderQueues renderQueues;

    renderQueues.setTargetRenderViewCount(7);
    QVERIFY(renderQueues.targetRenderViewCount() == 7);
    QVERIFY(renderQueues.currentRenderViewCount()== 0);
    QVERIFY(renderQueues.queuedFrames() == 0);
}

void tst_RenderQueues::circleQueues()
{
    Qt3D::Render::RenderQueues renderQueues;
    renderQueues.setTargetRenderViewCount(7);

    for (int j = 0; j < 10; j++) {
        QList<Qt3D::Render::RenderView *> renderViews;

        for (int i = 0; i < 7; i++) {
            renderViews << new Qt3D::Render::RenderView();
            renderQueues.queueRenderView(renderViews.at(i), i);
        }
        renderQueues.pushFrameQueue();
        if (renderQueues.queuedFrames() == renderQueues.framesCapacity())
        {
            renderQueues.popFrameQueue();
            QCOMPARE(renderQueues.queuedFrames(), renderQueues.framesCapacity() - 1);
        }
    }
}

void tst_RenderQueues::checkOrder()
{
    Qt3D::Render::RenderQueues renderQueues;
    renderQueues.setTargetRenderViewCount(7);

    QVector<Qt3D::Render::RenderView *> renderViews(7);

    for (int i = 0; i < 7; i++) {
        int processingOrder = (i % 2 == 0) ? (6 - i) : i;
        renderViews[processingOrder] = new Qt3D::Render::RenderView();
        renderQueues.queueRenderView(renderViews[processingOrder], processingOrder);
    }
}

void tst_RenderQueues::checkTimeToSubmit()
{
    Qt3D::Render::RenderQueues renderQueues;
    renderQueues.setTargetRenderViewCount(7);

    QVector<Qt3D::Render::RenderView *> renderViews(7);

    for (int i = 0; i < 7; i++) {
        int processingOrder = (i % 2 == 0) ? (6 - i) : i;
        renderViews[processingOrder] = new Qt3D::Render::RenderView();
        renderQueues.queueRenderView(renderViews[processingOrder], processingOrder);
        if (i < 6)
            QVERIFY(!renderQueues.isFrameQueueComplete());
        else
            QVERIFY(renderQueues.isFrameQueueComplete());
    }
}

void tst_RenderQueues::submitRenderViews()
{
    Qt3D::Render::RenderQueues renderQueues;
    renderQueues.setTargetRenderViewCount(7);

    QVector<Qt3D::Render::RenderView *> renderViews(7);

    for (int i = 0; i < 7; i++) {
        int processingOrder = (i % 2 == 0) ? (6 - i) : i;
        renderViews[processingOrder] = new Qt3D::Render::RenderView();
        renderQueues.queueRenderView(renderViews[processingOrder], processingOrder);
        if (i < 6)
            QVERIFY(!renderQueues.isFrameQueueComplete());
        else
            QVERIFY(renderQueues.isFrameQueueComplete());
    }

    renderQueues.pushFrameQueue();
    QVERIFY(renderQueues.currentRenderViewCount() == 0);
    QVERIFY(renderQueues.targetRenderViewCount() == 7);
}


class SimpleWorker : public QThread
{
    Q_OBJECT
public:
    QWaitCondition m_waitTimeToSubmit;
    QWaitCondition m_waitToFillQueue;
    QMutex m_mutex;
    Qt3D::Render::RenderQueues *m_renderQueues;

public slots:

    void run() // In Thread
    {
        for (int i = 0; i < 5; i++) {
            m_mutex.lock();
            m_waitToFillQueue.wait(&m_mutex);
            m_mutex.unlock();
            for (int j = 0; j < 3; j++) {
                QVERIFY(m_renderQueues->currentRenderViewCount() == 0);
                QVERIFY(!m_renderQueues->isFrameQueueComplete());

                for (int i = 0; i < 7; i++) {
                    QCOMPARE(m_renderQueues->currentRenderViewCount(), i);
                    m_renderQueues->queueRenderView(new Qt3D::Render::RenderView(), i);
                    QVERIFY(m_renderQueues->targetRenderViewCount() == 7);
                    QCOMPARE(m_renderQueues->currentRenderViewCount(), i + 1);
                    if (i < 6)
                        QVERIFY(!m_renderQueues->isFrameQueueComplete());
                    QThread::msleep(20); // Simulates business
                }
                QVERIFY(m_renderQueues->isFrameQueueComplete());
                m_renderQueues->pushFrameQueue();
            }
            m_waitTimeToSubmit.wakeOne();
        }
    }
};


void tst_RenderQueues::concurrentQueueAccess()
{
    Qt3D::Render::RenderQueues *renderQueues = new Qt3D::Render::RenderQueues;

    SimpleWorker *jobsThread = new SimpleWorker();
    renderQueues->setTargetRenderViewCount(7);
    jobsThread->m_renderQueues = renderQueues;

    QVERIFY(jobsThread->m_renderQueues->targetRenderViewCount() == renderQueues->targetRenderViewCount());
    QVERIFY(jobsThread->m_renderQueues->currentRenderViewCount() == renderQueues->currentRenderViewCount());

    // Start thread
    jobsThread->start();

    QThread::msleep(500); // To be sure the thread is properly started

    jobsThread->m_waitToFillQueue.wakeAll();

    for (int i = 0; i < 5; i++) {
        jobsThread->m_mutex.lock();
        jobsThread->m_waitTimeToSubmit.wait(&jobsThread->m_mutex);
        jobsThread->m_mutex.unlock();

        int count = 0;
        while (renderQueues->queuedFrames() > 0) {
            QVector<Qt3D::Render::RenderView *> renderViews = renderQueues->nextFrameQueue();
            qDeleteAll(renderViews);
            count++;
            renderQueues->popFrameQueue();
        }
        QCOMPARE(count, 3);
        jobsThread->m_waitToFillQueue.wakeAll();
    }
    jobsThread->wait();
}

void tst_RenderQueues::resetQueues()
{
    Qt3D::Render::RenderQueues renderQueues(12);

    renderQueues.setTargetRenderViewCount(5);
    for (int j = 0; j < 5; j++) {
        QList<Qt3D::Render::RenderView *> renderViews;

        for (int i = 0; i < 5; i++) {
            renderViews << new Qt3D::Render::RenderView();
            renderQueues.queueRenderView(renderViews.at(i), i);
        }
        renderQueues.pushFrameQueue();
    }
    QVERIFY(renderQueues.queuedFrames() == 5);
    QVERIFY(renderQueues.framesCapacity() == 12);
    renderQueues.reset();
    QVERIFY(renderQueues.queuedFrames() == 0);
    QVERIFY(renderQueues.currentRenderViewCount() == 0);
    QVERIFY(renderQueues.framesCapacity() == 12);
}

QTEST_APPLESS_MAIN(tst_RenderQueues)

#include "tst_renderqueues.moc"
