/* -*- C++ -*-

   This file contains a testsuite for global queue customizations in ThreadWeaver.

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

#include <QString>
#include <QtTest>

#include <ThreadWeaver/ThreadWeaver>
#include <ThreadWeaver/QueueSignals>
#include <ThreadWeaver/Queue>
#include <weaver_p.h>
#include <ThreadWeaver/IdDecorator>

using namespace ThreadWeaver;
QAtomicInt counter;

class CountingJobDecorator : public IdDecorator
{
public:
    explicit CountingJobDecorator(const JobPointer &job)
        : IdDecorator(job.data(), false)
        , original_(job)
    {}

    void run(JobPointer self, Thread *thread) Q_DECL_OVERRIDE {
        counter.fetchAndAddRelease(1);
        IdDecorator::run(self, thread);
        counter.fetchAndAddAcquire(1);
    }

    JobPointer original_;
};

class JobCountingWeaver : public Weaver
{
    Q_OBJECT
public:
    explicit JobCountingWeaver(QObject *parent = 0) : Weaver(parent) {}
    void enqueue(const QVector<JobPointer> &jobs) Q_DECL_OVERRIDE {
        QVector<JobPointer> decorated;
        std::transform(jobs.begin(), jobs.end(), std::back_inserter(decorated),
        [](const JobPointer & job)
        {
            return JobPointer(new CountingJobDecorator(job));
        });
        Weaver::enqueue(decorated);
    }
};

class CountingGlobalQueueFactory : public Queue::GlobalQueueFactory
{
    Queue *create(QObject *parent = 0) Q_DECL_OVERRIDE {
        return new Queue(new JobCountingWeaver, parent);
    }
};

int argc = 0;

class QueueFactoryTests : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testQueueFactory()
    {
        counter.storeRelease(0);
        QCoreApplication app(argc, (char **)0);
        Queue queue(new JobCountingWeaver(this));
        queue.enqueue(make_job([]() {}));  // nop
        queue.finish();
        QCOMPARE(counter.loadAcquire(), 2);
    }

    void testGlobalQueueFactory()
    {
        Queue::setGlobalQueueFactory(new CountingGlobalQueueFactory());
        QCoreApplication app(argc, (char **)0);
        counter.storeRelease(0);
        Queue::instance()->enqueue(make_job([]() {}));  // nop
        Queue::instance()->finish();
        QCOMPARE(counter.loadAcquire(), 2);
    }
};

QTEST_APPLESS_MAIN(QueueFactoryTests)

#include "QueueFactoryTests.moc"
