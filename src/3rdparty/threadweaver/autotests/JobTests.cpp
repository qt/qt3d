/* -*- C++ -*-

   This file contains a testsuite for job processing in ThreadWeaver.

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

#include "JobTests.h"

#include <cstdlib>

#include <QtCore/QMutex>
#include <QtTest/QtTest>
#include <QSignalSpy>

#include <ThreadWeaver/Queueing>
#include <ThreadWeaver/QueueStream>
#include <ThreadWeaver/Sequence>
#include <ThreadWeaver/Lambda>
#include <ThreadWeaver/ThreadWeaver>
#include <ThreadWeaver/Thread>
#include <ThreadWeaver/DebuggingAids>
#include <ThreadWeaver/Collection>
#include <ThreadWeaver/ResourceRestrictionPolicy>
#include <ThreadWeaver/Dependency>
#include <ThreadWeaver/DependencyPolicy>
#include <ThreadWeaver/QObjectDecorator>
#include <ThreadWeaver/Exception>

#include "AppendCharacterJob.h"
#include "AppendCharacterAndVerifyJob.h"

QMutex s_GlobalMutex;

using namespace ThreadWeaver;

//Ensure that after the object is created, the weaver is idle and resumed.
//Upon destruction, ensure the weaver is idle and suspended.
class WaitForIdleAndFinished
{
public:
    explicit WaitForIdleAndFinished(Queue *weaver)
        : weaver_(weaver)
    {
        Q_ASSERT(weaver);
        weaver_->finish();
        Q_ASSERT(weaver_->isIdle());
        weaver_->resume();
    }

    ~WaitForIdleAndFinished()
    {
        weaver_->resume();
        weaver_->dequeue();
        weaver_->finish();
        weaver_->suspend();
        Q_ASSERT(weaver_->isIdle());
    }
private:
    Queue *weaver_;
};

void JobTests::initTestCase()
{
    setDebugLevel(true, 1);
}

// Call finish() before leaving a test or use a WaitForIdleAndFinished object to make sure the queue is empty
// and in an idle state.

void JobTests::WeaverLazyThreadCreationTest()
{
    Queue weaver;
    QString sequence;

    WaitForIdleAndFinished w(&weaver);
    Q_ASSERT(weaver.isIdle());
    QCOMPARE(weaver.currentNumberOfThreads(), 0);
    weaver.stream() << new AppendCharacterJob(QChar('a'), &sequence);
    weaver.finish();
    QCOMPARE(weaver.currentNumberOfThreads(), 1);
    Q_ASSERT(weaver.isIdle());
}

void JobTests::SimpleJobTest()
{
    QString sequence;

    WaitForIdleAndFinished w(Queue::instance());
    stream() << new AppendCharacterJob(QChar('1'), &sequence);
    Queue::instance()->finish();
    QCOMPARE(sequence, QString("1"));
}

void JobTests::SimpleJobCollectionTest()
{
    QString sequence;
    Collection jobCollection;
    jobCollection << new AppendCharacterJob(QChar('a'), &sequence)
                  << new AppendCharacterJob(QChar('b'), &sequence)
                  << new AppendCharacterJob(QChar('c'), &sequence);

    WaitForIdleAndFinished w(Queue::instance());
    stream() << jobCollection;

    Queue::instance()->finish();

    QVERIFY(sequence.length() == 3);
    QVERIFY(sequence.count('a') == 1);
    QVERIFY(sequence.count('b') == 1);
    QVERIFY(sequence.count('c') == 1);
}

void JobTests::EmptyJobCollectionTest()
{
    Collection collection;

    WaitForIdleAndFinished w(Queue::instance());
    Q_ASSERT(Queue::instance()->isIdle());
    stream() << collection;
    Queue::instance()->finish();
    QVERIFY(collection.isFinished());
    QVERIFY(Queue::instance()->isIdle());
}

void JobTests::CollectionQueueingTest()
{
    QString output;
    Collection jobCollection;
    jobCollection << new AppendCharacterJob(QChar('a'), &output)
                  << new AppendCharacterJob(QChar('b'), &output)
                  << new AppendCharacterJob(QChar('c'), &output);

    Queue weaver;
    WaitForIdleAndFinished w(&weaver);
    weaver.suspend();
    weaver.stream() << jobCollection;
    QCOMPARE(weaver.queueLength(), 1); //collection queues itself, and it's elements upon execution of self
    weaver.resume();
    weaver.finish();
    QCOMPARE(output.length(), 3);
    QVERIFY(Queue::instance()->isIdle());
}

namespace
{
using namespace ThreadWeaver;

QString SequenceTemplate = "abcdefghijklmnopqrstuvwxyz";

class GeneratingCollection : public Collection
{
public:
    void run(JobPointer, Thread *) Q_DECL_OVERRIDE {
        std::for_each(SequenceTemplate.cbegin(), SequenceTemplate.cend(),
        [this](QChar it)
        {
            *this << new AppendCharacterJob(it, &sequence_);
        });
    }
    QString sequence_;
};

class GeneratingSequence : public Sequence
{
public:
    void run(JobPointer, Thread *) Q_DECL_OVERRIDE {
        std::for_each(SequenceTemplate.cbegin(), SequenceTemplate.cend(),
        [this](QChar it)
        {
            *this << new AppendCharacterJob(it, &sequence_);
        });
    }
    QString sequence_;
};

}

void JobTests::GeneratingCollectionTest()
{
    using namespace ThreadWeaver;

    GeneratingCollection collection;
    WaitForIdleAndFinished w(Queue::instance());
    stream() << make_job_raw(&collection);
    Queue::instance()->finish();
    QCOMPARE(collection.sequence_.count(), SequenceTemplate.length());
}

void JobTests::ShortJobSequenceTest()
{
    QString sequence;
    JobPointer jobA(new AppendCharacterJob(QChar('a'), &sequence));
    JobPointer jobB(new AppendCharacterJob(QChar('b'), &sequence));
    JobPointer jobC(new AppendCharacterJob(QChar('c'), &sequence));
    QSharedPointer<Sequence> jobSequence(new Sequence());
    jobSequence->addJob(jobA);
    jobSequence->addJob(jobB);
    jobSequence->addJob(jobC);

    WaitForIdleAndFinished w(Queue::instance());
    Queue::instance()->enqueue(jobSequence);
    // Job::DumpJobDependencies();
    Queue::instance()->finish();
    QCOMPARE(sequence, QLatin1String("abc"));
    QVERIFY(Queue::instance()->isIdle());
}

void JobTests::EmptyJobSequenceTest()
{
    using namespace ThreadWeaver;
    QObjectDecorator sequence(new Sequence());
    WaitForIdleAndFinished w(Queue::instance()); Q_UNUSED(w);
    Q_ASSERT(Queue::instance()->isIdle());
    QSignalSpy doneSignalSpy(&sequence, SIGNAL(done(ThreadWeaver::JobPointer)));
    QCOMPARE(doneSignalSpy.count(), 0);
    enqueue_raw(&sequence);
    Queue::instance()->finish();
    QVERIFY(sequence.isFinished());
    QVERIFY(Queue::instance()->isIdle());
    QCOMPARE(doneSignalSpy.count(), 1);
}

void JobTests::GeneratingSequenceTest()
{
    using namespace ThreadWeaver;

    GeneratingSequence sequence;
    WaitForIdleAndFinished w(Queue::instance());
    stream() << make_job_raw(&sequence);
    Queue::instance()->finish();
    QCOMPARE(sequence.sequence_, SequenceTemplate);
}

/** This test verifies that the done signal for a collection is only sent after all element of the collection have completed. */
void JobTests::IncompleteCollectionTest()
{
    using namespace ThreadWeaver;

    QString result;
    QObjectDecorator jobA(new AppendCharacterJob(QChar('a'), &result));
    AppendCharacterJob jobB(QChar('b'), &result); //jobB does not get added to the sequence and queued
    QObjectDecorator col(new Collection());
    *col.collection() << jobA;

    WaitForIdleAndFinished w(Queue::instance());
    DependencyPolicy::instance().addDependency(Dependency(&jobA, &jobB));
    QSignalSpy collectionDoneSignalSpy(&col, SIGNAL(done(ThreadWeaver::JobPointer)));
    QSignalSpy jobADoneSignalSpy(&jobA, SIGNAL(done(ThreadWeaver::JobPointer)));
    QCOMPARE(collectionDoneSignalSpy.count(), 0);
    QCOMPARE(jobADoneSignalSpy.count(), 0);
    enqueue_raw(&col);
    Queue::instance()->resume();
    QCoreApplication::processEvents();
    QCOMPARE(collectionDoneSignalSpy.count(), 0);
    QCOMPARE(jobADoneSignalSpy.count(), 0);
    DependencyPolicy::instance().removeDependency(Dependency(&jobA, &jobB));
    Queue::instance()->finish();
    QCoreApplication::processEvents();
    QVERIFY(col.collection()->isFinished());
    QVERIFY(Queue::instance()->isIdle());
    QCOMPARE(collectionDoneSignalSpy.count(), 1);
    QCOMPARE(jobADoneSignalSpy.count(), 1);
}

/** This test verifies that started() is emitted for a collection at the time the first of any elements of the collection gets
 * executed. */
void JobTests::EmitStartedOnFirstElementTest()
{
    using namespace ThreadWeaver;

    WaitForIdleAndFinished w(Queue::instance());
    Queue::instance()->suspend();
    QString result;

    JobPointer jobA(new AppendCharacterJob(QChar('a'), &result));
    JobPointer jobB(new AppendCharacterJob(QChar('b'), &result));
    QObjectDecorator collection(new Collection());
    Collection *decorated = dynamic_cast<Collection *>(collection.job());
    QVERIFY(decorated != 0);
    decorated->addJob(jobA);
    decorated->addJob(jobB);

    enqueue(make_job_raw(&collection));
    QSignalSpy collectionStartedSignalSpy(&collection, SIGNAL(started(ThreadWeaver::JobPointer)));
    QSignalSpy collectionDoneSignalSpy(&collection, SIGNAL(done(ThreadWeaver::JobPointer)));
    Queue::instance()->resume();
    QCoreApplication::processEvents();
    Queue::instance()->finish();
    QVERIFY(collection.isFinished());
    QCOMPARE(result.length(), 2);
    for (int i = 0; i < 100; ++i) {
        if (collectionStartedSignalSpy.count() != 0 && collectionDoneSignalSpy.count() != 0) {
            break;
        }
        QTest::qWait(1);
        debug(2, "JobTests::EmitStartedOnFirstElementTest: waiting (%i)\n", i);
        qApp->processEvents();
    }
    QCOMPARE(collectionStartedSignalSpy.count(), 1);
    QCOMPARE(collectionDoneSignalSpy.count(), 1);
    QVERIFY(Queue::instance()->isIdle());
}

/* This test verifies that all elements of a collection are only executed after all dependencies for the collection
 * itself have been resolved.
 * Previous tests have already verified that collections without dependencies get executed right away. */
void JobTests::CollectionDependenciesTest()
{
    using namespace ThreadWeaver;

    QString result;
    // set up a collection that depends on jobC which does not get queued
    JobPointer jobA(new AppendCharacterJob(QChar('a'), &result));
    JobPointer jobB(new AppendCharacterJob(QChar('b'), &result));
    QObjectDecorator col(new Collection());
    QSignalSpy collectionStartedSignalSpy(&col, SIGNAL(started(ThreadWeaver::JobPointer)));
    col.collection()->addJob(jobA);
    col.collection()->addJob(jobB);
    QEventLoop loop;
    connect(&col, SIGNAL(started(ThreadWeaver::JobPointer)), &loop, SLOT(quit()));

    QSharedPointer<AppendCharacterJob> jobC(new AppendCharacterJob(QChar('c'), &result));
    DependencyPolicy::instance().addDependency(Dependency(&col, jobC));

    // queue collection, but not jobC, the collection should not be executed
    WaitForIdleAndFinished w(Queue::instance()); Q_UNUSED(w);
    Queue::instance()->suspend();
    enqueue_raw(&col);
    Queue::instance()->resume();
    QCoreApplication::processEvents();
    QTest::qWait(100);
    //FIXME verify: dfaure needed this here: QTRY_COMPARE(collectionStartedSignalSpy.count(), 0);
    QCOMPARE(collectionStartedSignalSpy.count(), 0);
    // enqueue jobC, first jobC then the collection should be executed
    Queue::instance()->enqueue(jobC);
    QCoreApplication::processEvents();
    Queue::instance()->finish();
    QVERIFY(col.isFinished());
    QVERIFY(result.startsWith(jobC->character()));
    //QSKIP("This test is too fragile"); // PENDING(Mirko): fix
    //QTRY_COMPARE(collectionStartedSignalSpy.count(), 1);
    loop.exec();
    qApp->processEvents();
    QCOMPARE(collectionStartedSignalSpy.count(), 1);
    QVERIFY(Queue::instance()->isIdle());
}

void JobTests::QueueAndDequeueCollectionTest()
{
    QString sequence;
    JobPointer jobA(new AppendCharacterJob(QChar('a'), &sequence));
    JobPointer jobB(new AppendCharacterJob(QChar('b'), &sequence));
    JobPointer jobC(new AppendCharacterJob(QChar('c'), &sequence));
    QSharedPointer<Collection> collection(new Collection());
    collection->addJob(jobA);
    collection->addJob(jobB);
    collection->addJob(jobC);

    WaitForIdleAndFinished w(Queue::instance());
    Queue::instance()->suspend();

    Queue::instance()->enqueue(collection);
    Queue::instance()->dequeue(collection);
    QVERIFY(Queue::instance()->isEmpty());
}

void JobTests::QueueAndDequeueSequenceTest()
{
    QString sequence;
    JobPointer jobA(new AppendCharacterJob(QChar('a'), &sequence));
    JobPointer jobB(new AppendCharacterJob(QChar('b'), &sequence));
    JobPointer jobC(new AppendCharacterJob(QChar('c'), &sequence));
    QSharedPointer<Sequence> jobSequence(new Sequence());
    jobSequence->addJob(jobA);
    jobSequence->addJob(jobB);
    jobSequence->addJob(jobC);

    WaitForIdleAndFinished w(Queue::instance());
    Queue::instance()->suspend();

    Queue::instance()->enqueue(jobSequence);
    Queue::instance()->dequeue(jobSequence);
    QVERIFY(Queue::instance()->isEmpty());
}

void JobTests::BlockingExecuteTest()
{
    QString sequence;
    AppendCharacterJob job(QChar('a'), &sequence);
    job.blockingExecute();
    QCOMPARE(sequence, QString("a"));
}

void JobTests::RecursiveSequenceTest()
{
    QString sequence;
    JobPointer jobA(new AppendCharacterJob(QChar('a'), &sequence));
    JobPointer jobB(new AppendCharacterJob(QChar('b'), &sequence));
    JobPointer jobC(new AppendCharacterJob(QChar('c'), &sequence));
    JobPointer jobD(new AppendCharacterJob(QChar('d'), &sequence));
    JobPointer jobE(new AppendCharacterJob(QChar('e'), &sequence));
    JobPointer jobF(new AppendCharacterJob(QChar('f'), &sequence));
    JobPointer jobG(new AppendCharacterJob(QChar('g'), &sequence));
    JobPointer jobH(new AppendCharacterJob(QChar('h'), &sequence));
    JobPointer jobI(new AppendCharacterJob(QChar('i'), &sequence));
    JobPointer jobJ(new AppendCharacterJob(QChar('j'), &sequence));
    QSharedPointer<Sequence> jobSequence1(new Sequence());
    jobSequence1->addJob(jobA);
    jobSequence1->addJob(jobB);
    jobSequence1->addJob(jobC);
    QSharedPointer<Sequence> jobSequence2(new Sequence());
    jobSequence2->addJob(jobD);
    jobSequence2->addJob(jobE);
    jobSequence2->addJob(jobF);
    QSharedPointer<Sequence> jobSequence3(new Sequence());
    jobSequence3->addJob(jobG);
    jobSequence3->addJob(jobH);
    jobSequence3->addJob(jobI);
    jobSequence3->addJob(jobJ);
    // sequence 4 will contain sequences 1, 2, and 3, in that order:
    QSharedPointer<Sequence> jobSequence4(new Sequence());
    jobSequence4->addJob(jobSequence1);
    jobSequence4->addJob(jobSequence2);
    jobSequence4->addJob(jobSequence3);

    WaitForIdleAndFinished w(Queue::instance());
    Queue::instance()->suspend();
    Queue::instance()->enqueue(jobSequence4);
//    DependencyPolicy::instance().dumpJobDependencies();
    Queue::instance()->resume();
    Queue::instance()->finish();
    QCOMPARE(sequence, QLatin1String("abcdefghij"));
}

void JobTests::RecursiveQueueAndDequeueCollectionTest()
{
    QString sequence;
    JobPointer jobA(new AppendCharacterJob(QChar('a'), &sequence));
    JobPointer jobB(new AppendCharacterJob(QChar('b'), &sequence));
    JobPointer jobC(new AppendCharacterJob(QChar('c'), &sequence));
    JobPointer jobD(new AppendCharacterJob(QChar('d'), &sequence));
    JobPointer jobE(new AppendCharacterJob(QChar('e'), &sequence));
    JobPointer jobF(new AppendCharacterJob(QChar('f'), &sequence));
    JobPointer jobG(new AppendCharacterJob(QChar('g'), &sequence));
    JobPointer jobH(new AppendCharacterJob(QChar('h'), &sequence));
    JobPointer jobI(new AppendCharacterJob(QChar('i'), &sequence));
    JobPointer jobJ(new AppendCharacterJob(QChar('j'), &sequence));
    QSharedPointer<Collection> collection1(new Collection());
    collection1->addJob(jobA);
    collection1->addJob(jobB);
    collection1->addJob(jobC);
    QSharedPointer<Collection> collection2(new Collection());
    collection2->addJob(jobD);
    collection2->addJob(jobE);
    collection2->addJob(jobF);
    QSharedPointer<Collection> collection3(new Collection());
    collection3->addJob(jobG);
    collection3->addJob(jobH);
    collection3->addJob(jobI);
    collection3->addJob(jobJ);
    // sequence 4 will contain sequences 1, 2, and 3, in that order:
    QSharedPointer<Collection> collection4(new Collection());
    collection4->addJob(collection1);
    collection4->addJob(collection2);
    collection4->addJob(collection3);

    WaitForIdleAndFinished w(Queue::instance());
    Queue::instance()->suspend();
    Queue::instance()->enqueue(collection4);
    Queue::instance()->dequeue(collection4);
    QVERIFY(Queue::instance()->isEmpty());
    Queue::instance()->resume();
}

void JobTests::RecursiveQueueAndDequeueSequenceTest()
{
    QString sequence;
    JobPointer jobA(new AppendCharacterJob(QChar('a'), &sequence));
    JobPointer jobB(new AppendCharacterJob(QChar('b'), &sequence));
    JobPointer jobC(new AppendCharacterJob(QChar('c'), &sequence));
    JobPointer jobD(new AppendCharacterJob(QChar('d'), &sequence));
    JobPointer jobE(new AppendCharacterJob(QChar('e'), &sequence));
    JobPointer jobF(new AppendCharacterJob(QChar('f'), &sequence));
    JobPointer jobG(new AppendCharacterJob(QChar('g'), &sequence));
    JobPointer jobH(new AppendCharacterJob(QChar('h'), &sequence));
    JobPointer jobI(new AppendCharacterJob(QChar('i'), &sequence));
    JobPointer jobJ(new AppendCharacterJob(QChar('j'), &sequence));
    QSharedPointer<Sequence> jobSequence1(new Sequence());
    jobSequence1->addJob(jobA);
    jobSequence1->addJob(jobB);
    jobSequence1->addJob(jobC);
    QSharedPointer<Sequence> jobSequence2(new Sequence());
    jobSequence2->addJob(jobD);
    jobSequence2->addJob(jobE);
    jobSequence2->addJob(jobF);
    QSharedPointer<Sequence> jobSequence3(new Sequence());
    jobSequence3->addJob(jobG);
    jobSequence3->addJob(jobH);
    jobSequence3->addJob(jobI);
    jobSequence3->addJob(jobJ);
    // sequence 4 will contain sequences 1, 2, and 3, in that order:
    QSharedPointer<Sequence> jobSequence4(new Sequence());
    jobSequence4->addJob(jobSequence1);
    jobSequence4->addJob(jobSequence2);
    jobSequence4->addJob(jobSequence3);

    WaitForIdleAndFinished w(Queue::instance());
    Queue::instance()->suspend();
    Queue::instance()->enqueue(jobSequence4);
    Queue::instance()->dequeue(jobSequence4);
    QVERIFY(Queue::instance()->isEmpty());
    Queue::instance()->resume();
}

void JobTests::QueueAndDequeueAllCollectionTest()
{
    QString sequence;
    JobPointer jobA(new AppendCharacterJob(QChar('a'), &sequence));
    JobPointer jobB(new AppendCharacterJob(QChar('b'), &sequence));
    JobPointer jobC(new AppendCharacterJob(QChar('c'), &sequence));
    QSharedPointer<Collection> collection(new Collection());
    collection->addJob(jobA);
    collection->addJob(jobB);
    collection->addJob(jobC);

    WaitForIdleAndFinished w(Queue::instance());
    Queue::instance()->suspend();
    QVERIFY(Queue::instance()->isEmpty());
    Queue::instance()->enqueue(collection);
    //collection cannot have been started, so only one job is queued at the moment:
    QCOMPARE(Queue::instance()->queueLength(), 1);
    Queue::instance()->dequeue();
    QVERIFY(Queue::instance()->isEmpty());
}

void JobTests::QueueAndDequeueAllSequenceTest()
{
    QString sequence;
    JobPointer jobA(new AppendCharacterJob(QChar('a'), &sequence));
    JobPointer jobB(new AppendCharacterJob(QChar('b'), &sequence));
    JobPointer jobC(new AppendCharacterJob(QChar('c'), &sequence));
    QSharedPointer<Sequence> jobSequence(new Sequence());
    jobSequence->addJob(jobA);
    jobSequence->addJob(jobB);
    jobSequence->addJob(jobC);

    WaitForIdleAndFinished w(Queue::instance());
    Queue::instance()->suspend();

    Queue::instance()->enqueue(jobSequence);
    Queue::instance()->dequeue();
    QVERIFY(Queue::instance()->isEmpty());
}

void JobTests::RecursiveQueueAndDequeueAllCollectionTest()
{
    QString sequence;
    JobPointer jobA(new AppendCharacterJob(QChar('a'), &sequence));
    JobPointer jobB(new AppendCharacterJob(QChar('b'), &sequence));
    JobPointer jobC(new AppendCharacterJob(QChar('c'), &sequence));
    JobPointer jobD(new AppendCharacterJob(QChar('d'), &sequence));
    JobPointer jobE(new AppendCharacterJob(QChar('e'), &sequence));
    JobPointer jobF(new AppendCharacterJob(QChar('f'), &sequence));
    JobPointer jobG(new AppendCharacterJob(QChar('g'), &sequence));
    JobPointer jobH(new AppendCharacterJob(QChar('h'), &sequence));
    JobPointer jobI(new AppendCharacterJob(QChar('i'), &sequence));
    JobPointer jobJ(new AppendCharacterJob(QChar('j'), &sequence));
    QSharedPointer<Collection> collection1(new Collection());
    collection1->addJob(jobA);
    collection1->addJob(jobB);
    collection1->addJob(jobC);
    QSharedPointer<Collection> collection2(new Collection());
    collection2->addJob(jobD);
    collection2->addJob(jobE);
    collection2->addJob(jobF);
    QSharedPointer<Collection> collection3(new Collection());
    collection3->addJob(jobG);
    collection3->addJob(jobH);
    collection3->addJob(jobI);
    collection3->addJob(jobJ);
    // sequence 4 will contain sequences 1, 2, and 3, in that order:
    QSharedPointer<Collection> collection4(new Collection());
    collection4->addJob(collection1);
    collection4->addJob(collection2);
    collection4->addJob(collection3);

    WaitForIdleAndFinished w(Queue::instance());
    Queue::instance()->suspend();
    Queue::instance()->enqueue(collection4);
    Queue::instance()->dequeue();
    QVERIFY(Queue::instance()->isEmpty());
    Queue::instance()->resume();
    Queue::instance()->finish();

}

void JobTests::RecursiveQueueAndDequeueAllSequenceTest()
{
    QString sequence;
    JobPointer jobA(new AppendCharacterJob(QChar('a'), &sequence));
    JobPointer jobB(new AppendCharacterJob(QChar('b'), &sequence));
    JobPointer jobC(new AppendCharacterJob(QChar('c'), &sequence));
    JobPointer jobD(new AppendCharacterJob(QChar('d'), &sequence));
    JobPointer jobE(new AppendCharacterJob(QChar('e'), &sequence));
    JobPointer jobF(new AppendCharacterJob(QChar('f'), &sequence));
    JobPointer jobG(new AppendCharacterJob(QChar('g'), &sequence));
    JobPointer jobH(new AppendCharacterJob(QChar('h'), &sequence));
    JobPointer jobI(new AppendCharacterJob(QChar('i'), &sequence));
    JobPointer jobJ(new AppendCharacterJob(QChar('j'), &sequence));
    QSharedPointer<Sequence> jobSequence1(new Sequence());
    jobSequence1->addJob(jobA);
    jobSequence1->addJob(jobB);
    jobSequence1->addJob(jobC);
    QSharedPointer<Sequence> jobSequence2(new Sequence());
    jobSequence2->addJob(jobD);
    jobSequence2->addJob(jobE);
    jobSequence2->addJob(jobF);
    QSharedPointer<Sequence> jobSequence3(new Sequence());
    jobSequence3->addJob(jobG);
    jobSequence3->addJob(jobH);
    jobSequence3->addJob(jobI);
    jobSequence3->addJob(jobJ);
    // sequence 4 will contain sequences 1, 2, and 3, in that order:
    QSharedPointer<Sequence> jobSequence4(new Sequence());
    jobSequence4->addJob(jobSequence1);
    jobSequence4->addJob(jobSequence2);
    jobSequence4->addJob(jobSequence3);

    WaitForIdleAndFinished w(Queue::instance());
    Queue::instance()->suspend();
    Queue::instance()->enqueue(jobSequence4);
    Queue::instance()->dequeue();
    QVERIFY(Queue::instance()->isEmpty());
    Queue::instance()->resume();
    Queue::instance()->finish();
}

//     This test is not the most efficient, as the mutex locking takes most of
//     the execution time. Anyway, it will fail if the jobs are not executed
//     in the right order, and the order is randomized.
void JobTests::MassiveJobSequenceTest()
{
    const int NoOfChars = 1024;
    const char *Alphabet = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const int SizeOfAlphabet = strlen(Alphabet);
    AppendCharacterAndVerifyJob jobs[NoOfChars];
    Sequence jobSequence;
    QString sequence;
    QString in;

    srand(1);
    in.reserve(NoOfChars);
    sequence.reserve(NoOfChars);
    for (int i = 0; i < NoOfChars; ++i) {
        const int position = static_cast<int>(SizeOfAlphabet * ((1.0 * rand()) / RAND_MAX));
        Q_ASSERT(0 <= position && position < SizeOfAlphabet);
        QChar c(Alphabet[position]);
        in.append(c);
    }

    for (int i = 0; i < NoOfChars; ++i) {
        jobs[i].setValues(in.at(i), &sequence, in);
        jobSequence << jobs[i];
    }

    WaitForIdleAndFinished w(Queue::instance());
    QVERIFY(Queue::instance()->isIdle());
    enqueue_raw(&jobSequence);
    Queue::instance()->finish();
    QVERIFY(Queue::instance()->isIdle());
    QCOMPARE(sequence, in);
}

void JobTests::SimpleRecursiveSequencesTest()
{
    QString sequence;
    AppendCharacterJob jobB(QChar('b'), &sequence);
    Sequence jobSequence1;
    jobSequence1 << jobB;

    AppendCharacterJob jobC(QChar('c'), &sequence);
    AppendCharacterJob jobA(QChar('a'), &sequence);
    Sequence jobSequence2;
    jobSequence2 << jobA;
    jobSequence2 << jobSequence1;
    jobSequence2 << jobC;

    WaitForIdleAndFinished w(Queue::instance());
    enqueue_raw(&jobSequence2);
    Queue::instance()->finish();
    QCOMPARE(sequence, QString("abc"));
}

void JobTests::SequenceOfSequencesTest()
{
    QString sequence;
    AppendCharacterJob jobA(QChar('a'), &sequence);
    AppendCharacterJob jobB(QChar('b'), &sequence);
    AppendCharacterJob jobC(QChar('c'), &sequence);
    AppendCharacterJob jobD(QChar('d'), &sequence);
    AppendCharacterJob jobE(QChar('e'), &sequence);
    AppendCharacterJob jobF(QChar('f'), &sequence);
    AppendCharacterJob jobG(QChar('g'), &sequence);
    AppendCharacterJob jobH(QChar('h'), &sequence);
    AppendCharacterJob jobI(QChar('i'), &sequence);
    AppendCharacterJob jobJ(QChar('j'), &sequence);
    Sequence jobSequence1;
    jobSequence1 << jobA << jobB << jobC;
    Sequence jobSequence2;
    jobSequence2 << jobD << jobE << jobF;
    Sequence jobSequence3;
    jobSequence3 << jobG << jobH << jobI << jobJ;
    // sequence 4 will contain sequences 1, 2, and 3, in that order:
    Sequence jobSequence4;
    jobSequence4 << jobSequence1 << jobSequence2 << jobSequence3;

    WaitForIdleAndFinished w(Queue::instance());
    stream() << jobSequence4;
    // Job::DumpJobDependencies();
    Queue::instance()->finish();
    QCOMPARE(sequence, QString("abcdefghij"));
}

void JobTests::QueueAndStopTest()
{
    QString sequence;
    AppendCharacterJob a('a', &sequence);
    AppendCharacterJob b('b', &sequence);
    AppendCharacterJob c('c', &sequence);
    FailingAppendCharacterJob d('d', &sequence);
    AppendCharacterJob e('e', &sequence);
    AppendCharacterJob f('f', &sequence);
    AppendCharacterJob g('g', &sequence);
    Sequence jobSequence;
    jobSequence << a << b << c << d << e << f << g;

    WaitForIdleAndFinished w(Queue::instance());
    stream() << jobSequence;
    Queue::instance()->finish();
    QCOMPARE(sequence, QString("abcd"));
}

void JobTests::ResourceRestrictionPolicyBasicsTest()
{
    // this test tests that with resource restrictions assigned, jobs
    // still get executed as expected
    QString sequence;
    ResourceRestrictionPolicy restriction(2);
    AppendCharacterJob a('a', &sequence);
    AppendCharacterJob b('b', &sequence);
    AppendCharacterJob c('c', &sequence);
    AppendCharacterJob d('d', &sequence);
    AppendCharacterJob e('e', &sequence);
    AppendCharacterJob f('f', &sequence);
    AppendCharacterJob g('g', &sequence);
    Collection collection;
    collection << a << b << c << d << e << f << g;
    a.assignQueuePolicy(&restriction);
    b.assignQueuePolicy(&restriction);
    c.assignQueuePolicy(&restriction);
    d.assignQueuePolicy(&restriction);
    e.assignQueuePolicy(&restriction);
    f.assignQueuePolicy(&restriction);
    g.assignQueuePolicy(&restriction);

    WaitForIdleAndFinished w(Queue::instance());
    stream() << collection;
    Queue::instance()->finish();
    QVERIFY(Queue::instance()->isIdle());
}

void JobTests::jobStarted(JobPointer)
{
    // qDebug() << "jobStarted";
    QVERIFY(thread() == QThread::currentThread());
}

void JobTests::jobDone(JobPointer)
{
    // qDebug() << "jobDone";
    QVERIFY(thread() == QThread::currentThread());
}

void JobTests::JobSignalsAreEmittedAsynchronouslyTest()
{
    using namespace ThreadWeaver;

    char bits[] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g' };
    const int NumberOfBits = sizeof bits / sizeof bits[0];
    QString sequence;
    QObjectDecorator collection(new Collection, this);

    QVERIFY(connect(&collection, SIGNAL(started(ThreadWeaver::JobPointer)), SLOT(jobStarted(ThreadWeaver::JobPointer))));
    QVERIFY(connect(&collection, SIGNAL(done(ThreadWeaver::JobPointer)), SLOT(jobDone(ThreadWeaver::JobPointer))));
    for (int counter = 0; counter < NumberOfBits; ++counter) {
        QJobPointer job(new QObjectDecorator(new AppendCharacterJob(bits[counter], &sequence)));
        QVERIFY(connect(job.data(), SIGNAL(started(ThreadWeaver::JobPointer)), SLOT(jobStarted(ThreadWeaver::JobPointer))));
        QVERIFY(connect(job.data(), SIGNAL(done(ThreadWeaver::JobPointer)), SLOT(jobDone(ThreadWeaver::JobPointer))));
        collection.collection()->addJob(job);
    }

    WaitForIdleAndFinished w(Queue::instance());
    enqueue_raw(&collection);
    QCoreApplication::processEvents();
    Queue::instance()->finish();
    QVERIFY(sequence.length() == NumberOfBits);
}

QAtomicInt deliveryTestCounter;

void JobTests::deliveryTestJobDone(JobPointer)
{
    deliveryTestCounter.fetchAndAddRelease(-1);
}

void noOp() {}

void JobTests::JobSignalsDeliveryTest()
{
    //This test was added to investigate segmentation faults during signal delivery from jobs to the main thread.
    //Relies on processEvents() processing all pending events, as the specification says.
    using namespace ThreadWeaver;

    QCOMPARE(deliveryTestCounter.loadAcquire(), 0);
    WaitForIdleAndFinished w(Queue::instance());
    for (int count = 0; count < 100; ++count) {
        QJobPointer job(new QObjectDecorator(new Lambda<void(*)()>(noOp)));
        QVERIFY(connect(job.data(), SIGNAL(done(ThreadWeaver::JobPointer)), SLOT(deliveryTestJobDone(ThreadWeaver::JobPointer))));
        deliveryTestCounter.fetchAndAddRelease(1);
        Queue::instance()->enqueue(job);
    }
    QCoreApplication::processEvents();
    Queue::instance()->finish();
    QCoreApplication::processEvents();
    QCOMPARE(deliveryTestCounter.loadAcquire(), 0);
}

void decrementCounter()
{
    deliveryTestCounter.fetchAndAddRelease(-1);
}

void JobTests::JobPointerExecutionTest()
{
    //This test was added to investigate segmentation faults during signal delivery from jobs to the main thread.
    //Relies on processEvents() processing all pending events, as the specification says.
    using namespace ThreadWeaver;

    QCOMPARE(deliveryTestCounter.loadAcquire(), 0);
    WaitForIdleAndFinished w(Queue::instance());
    for (int count = 0; count < 100; ++count) {
        JobPointer job(new Lambda<void(*)()>(decrementCounter));
        deliveryTestCounter.fetchAndAddRelease(1);
        Queue::instance()->enqueue(job);
    }
    QCoreApplication::processEvents();
    Queue::instance()->finish();
    QCoreApplication::processEvents();
    QCOMPARE(deliveryTestCounter.loadAcquire(), 0);
}

void JobTests::DequeueSuspendedSequenceTest()
{
    using namespace ThreadWeaver;

    Sequence sequence;
    Queue weaver;
    weaver.suspend();
    enqueue_raw(&weaver, &sequence);
    weaver.dequeue();
    // don't crash
}

void JobTests::IdDecoratorDecoratesTest()
{
    using namespace ThreadWeaver;
    QString sequence;
    JobPointer job(new IdDecorator(new AppendCharacterJob('a', &sequence)));
    WaitForIdleAndFinished w(Queue::instance());
    Queue::instance()->enqueue(job);
    Queue::instance()->finish();
    QCOMPARE(sequence, QString::fromLatin1("a"));
}

void JobTests::IdDecoratorAutoDeleteTest()
{
    using namespace ThreadWeaver;
    IdDecorator id(0);
    QCOMPARE(id.autoDelete(), true); // autoDelete is on by default
    id.setAutoDelete(false);
    QCOMPARE(id.autoDelete(), false);
    id.setAutoDelete(true);
    QCOMPARE(id.autoDelete(), true);
    // now do not crash, even though id decorates a null pointer
}

void JobTests::IdDecoratorSingleAllocationTest()
{
    using namespace ThreadWeaver;

    struct DecoratedJob : public IdDecorator {
        QString sequence;
        AppendCharacterJob job;
        DecoratedJob() : IdDecorator(&job, false), job('a', &sequence) {}
    };

    WaitForIdleAndFinished w(Queue::instance());
    DecoratedJob job;
    enqueue_raw(&job);
    Queue::instance()->finish();
    QCOMPARE(job.sequence, QString::fromLatin1("a"));
}

struct InstanceCountedJob : public Job {
    static QAtomicInt counter;

    void run(JobPointer, Thread *)
    {
    }

    InstanceCountedJob()
    {
        counter.fetchAndAddRelease(1);
    }

    ~InstanceCountedJob()
    {
        counter.fetchAndAddRelease(-1);
    }
};

QAtomicInt InstanceCountedJob::counter;

/** @brief Verify that neither the queue nor the thread keep a reference to the job after completing it.
 *
 * This is necessary because user-allocated objects like queue policies may be registered with the jobs. If the jobs stick around
 * until the thread or queue are deleted, the user-allocatd objects may have gone out of scope or been deleted already, causing
 * potential errors. From ThreadWeaver's point of view, a job seizes to exist once the processing thread asks for the next job. */
void JobTests::JobsAreDestroyedAfterFinishTest()
{
    using namespace ThreadWeaver;
    WaitForIdleAndFinished w(Queue::instance()); Q_UNUSED(w);
    Queue::instance()->suspend();
    JobPointer job(new InstanceCountedJob);
    Queue::instance()->enqueue(job);
    QCOMPARE(InstanceCountedJob::counter.loadAcquire(), 1);
    Queue::instance()->resume();
    QCOMPARE(InstanceCountedJob::counter.loadAcquire(), 1);
    Queue::instance()->finish();
    QCOMPARE(InstanceCountedJob::counter.loadAcquire(), 1);
    QCoreApplication::processEvents();
    QCOMPARE(InstanceCountedJob::counter.loadAcquire(), 1);
    job.clear();
    // if this succeeds, job is the only shared pointer pointing to the created InstanceCountedJob object:
    QCOMPARE(InstanceCountedJob::counter.loadAcquire(), 0);
}

void JobTests::JobExitStatusByExceptionTest()
{
    using namespace ThreadWeaver;

    struct FailingJob : public Job {
        void run(JobPointer, Thread *) Q_DECL_OVERRIDE {
            throw JobFailed();
        }
    };

    FailingJob failing;
    failing.blockingExecute();
    QCOMPARE(failing.status(), Job::Status_Failed);

    struct AbortingJob : public Job {
        void run(JobPointer, Thread *) Q_DECL_OVERRIDE {
            throw JobAborted();
        }
    };

    AbortingJob aborting;
    aborting.blockingExecute();
    QCOMPARE(aborting.status(), Job::Status_Aborted);

    struct SuccessfulJob : public Job {
        void run(JobPointer, Thread *) Q_DECL_OVERRIDE {
            // do nothing
        }
    };
    SuccessfulJob successful;
    successful.blockingExecute();
    QCOMPARE(successful.status(), Job::Status_Success);
}

void JobTests::JobManualExitStatusTest()
{
    using namespace ThreadWeaver;

    struct FailingJob : public Job {
        void run(JobPointer, Thread *) Q_DECL_OVERRIDE {
            setStatus(Job::Status_Failed);
        }
    };

    FailingJob failing;
    failing.blockingExecute();
    QCOMPARE(failing.status(), Job::Status_Failed);

    struct AbortingJob : public Job {
        void run(JobPointer, Thread *) Q_DECL_OVERRIDE {
            setStatus(Job::Status_Aborted);
        }
    };

    AbortingJob aborting;
    aborting.blockingExecute();
    QCOMPARE(aborting.status(), Job::Status_Aborted);

    struct SuccessfulJob : public Job {
        void run(JobPointer, Thread *) Q_DECL_OVERRIDE {
            // do nothing
        }
    };
    SuccessfulJob successful;
    successful.blockingExecute();
    QCOMPARE(successful.status(), Job::Status_Success);
}

void JobTests::QueueStreamLifecycletest()
{
    QString sequence;
    using namespace ThreadWeaver;
    WaitForIdleAndFinished w(Queue::instance()); Q_UNUSED(w);
    stream() << make_job(new AppendCharacterJob('a', &sequence)) // enqueues JobPointer
             << new AppendCharacterJob('b', &sequence) // enqueues JobInterface*
             << make_job(new AppendCharacterJob('c', &sequence));
    Queue::instance()->finish();
    QCOMPARE(sequence.count(), 3);
}

QTEST_MAIN(JobTests)

