#include "QueueTests.h"

#include <QtCore/QChar>

#include <QtCore/QObject>
#include <QtCore/QThread>
#include <QtCore/QDebug>
#include <QtTest/QtTest>

#include "AppendCharacterJob.h"

#include <ThreadWeaver/Queueing>
/* -*- C++ -*-

   This file contains a testsuite for the queueing behaviour in ThreadWeaver.

   $ Author: Mirko Boehm $
   $ Copyright: (C) 2005-2013 Mirko Boehm $
   $ Contact: mirko@kde.org
         http://www.kde.org
         http://creative-destruction.me $

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.

*/

#include <ThreadWeaver/Job>
#include <ThreadWeaver/State>
#include <ThreadWeaver/QueuePolicy>
#include <ThreadWeaver/Sequence>
#include <ThreadWeaver/Collection>
#include <ThreadWeaver/DebuggingAids>
#include <ThreadWeaver/DependencyPolicy>
#include <ThreadWeaver/ResourceRestrictionPolicy>

#include <ThreadWeaver/ThreadWeaver>
#include <ThreadWeaver/Thread>
#include <ThreadWeaver/QObjectDecorator>

QMutex s_GlobalMutex;

LowPriorityAppendCharacterJob::LowPriorityAppendCharacterJob(QChar c, QString *stringref)
    : AppendCharacterJob(c, stringref)
{}

int LowPriorityAppendCharacterJob ::priority() const
{
    return -1;
}

HighPriorityAppendCharacterJob::HighPriorityAppendCharacterJob(QChar c, QString *stringref)
    : AppendCharacterJob(c, stringref)
{}

int HighPriorityAppendCharacterJob::priority() const
{
    return 1;
}

SecondThreadThatQueues::SecondThreadThatQueues()
    : QThread()
{
}

void SecondThreadThatQueues::run()
{
    QString sequence;
    AppendCharacterJob a('a', &sequence);

    ThreadWeaver::enqueue_raw(&a);
    ThreadWeaver::Queue::instance()->finish();
    QCOMPARE(sequence, QString("a"));
}

QueueTests::QueueTests(QObject *parent)
    : QObject(parent)
    , autoDeleteJob(0)
{
}

void QueueTests::initTestCase()
{
    ThreadWeaver::setDebugLevel(true,  1);
}

void QueueTests::SimpleQueuePrioritiesTest()
{
    using namespace ThreadWeaver;

    Queue weaver;
    weaver.setMaximumNumberOfThreads(1);    // just one thread
    QString sequence;
    LowPriorityAppendCharacterJob jobA(QChar('a'), &sequence);
    AppendCharacterJob jobB(QChar('b'), &sequence);
    HighPriorityAppendCharacterJob jobC(QChar('c'), &sequence);

    // queue low priority, then normal priority, then high priority
    // if priorities are processed correctly, the jobs will be executed in reverse order

    weaver.suspend();

    enqueue_raw(&weaver, &jobA);
    enqueue_raw(&weaver, &jobB);
    enqueue_raw(&weaver, &jobC);

    weaver.resume();
    weaver.finish();

    QCOMPARE(sequence, QString("cba"));
}

void QueueTests::WeaverInitializationTest()
{
    // this one mostly tests the sanity of the startup behaviour
    ThreadWeaver::Queue weaver;
    QCOMPARE(weaver.currentNumberOfThreads(), 0);
    QVERIFY(weaver.isEmpty());
    QVERIFY(weaver.isIdle());
    QVERIFY(weaver.queueLength() == 0);
    weaver.finish();
}

void QueueTests::QueueFromSecondThreadTest()
{
    ThreadWeaver::Queue::instance(); //create global instance in the main thread
    SecondThreadThatQueues thread;
    thread.start();
    thread.wait();
    QVERIFY(ThreadWeaver::Queue::instance()->isIdle());
}

void QueueTests::deleteJob(ThreadWeaver::JobPointer job)
{
    // test that signals are properly emitted (asynchronously, that is):
    QVERIFY(thread() == QThread::currentThread());
    QVERIFY(job == autoDeleteJob);
    delete autoDeleteJob; autoDeleteJob = 0;
}

void QueueTests::DeleteDoneJobsFromSequenceTest()
{
    using namespace ThreadWeaver;
    QString sequence;
    autoDeleteJob = new QObjectDecorator(new AppendCharacterJob(QChar('a'), &sequence));
    AppendCharacterJob b(QChar('b'), &sequence);
    AppendCharacterJob c(QChar('c'), &sequence);
    Collection collection;
    collection << make_job_raw(autoDeleteJob) << b << c;
    QVERIFY(autoDeleteJob != 0);
    QVERIFY(connect(autoDeleteJob, SIGNAL(done(ThreadWeaver::JobPointer)),
                    SLOT(deleteJob(ThreadWeaver::JobPointer))));
    stream() << collection;
    QTest::qWait(100); // return to event queue to make sure signals are delivered
    Queue::instance()->finish();
    QTest::qWait(100); // return to event queue to make sure signals are delivered
    // no need to delete a, that should be done in deleteJob
    QVERIFY(autoDeleteJob == 0);
}

void QueueTests::deleteCollection(ThreadWeaver::JobPointer collection)
{
    QVERIFY(thread() == QThread::currentThread());
    QVERIFY(collection == autoDeleteCollection);
    delete autoDeleteCollection; autoDeleteCollection = 0;
}

void QueueTests::DeleteCollectionOnDoneTest()
{
    using namespace ThreadWeaver;
    QString sequence;
    autoDeleteCollection = new QObjectDecorator(new Collection);
    QVERIFY(connect(autoDeleteCollection, SIGNAL(done(ThreadWeaver::JobPointer)),
                    SLOT(deleteCollection(ThreadWeaver::JobPointer))));

    AppendCharacterJob a(QChar('a'), &sequence);
    AppendCharacterJob b(QChar('b'), &sequence);
    *autoDeleteCollection->collection() << a << b;

    enqueue_raw(autoDeleteCollection);
    // return to event queue to make sure signals are delivered
    // (otherwise, no slot calls would happen before the end of this function)
    // I assume the amount of time that we wait does not matter
    QTest::qWait(10);
    Queue::instance()->finish();
    // return to event queue to make sure signals are delivered
    QTest::qWait(10);
    // no need to delete a, that should be done in deleteJob
    QVERIFY(sequence.length() == 2);
    QVERIFY(autoDeleteCollection == 0);
}

QTEST_MAIN(QueueTests)

