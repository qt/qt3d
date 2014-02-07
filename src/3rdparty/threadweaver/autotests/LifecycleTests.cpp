/* -*- C++ -*-

   This file contains a testsuite for JobPointer behaviour in ThreadWeaver.

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
#include <ThreadWeaver/JobPointer>
#include <ThreadWeaver/Job>

class NotifyOnDeletejob : public ThreadWeaver::Job
{
public:
    explicit NotifyOnDeletejob(bool &exists)
        : ThreadWeaver::Job()
        , m_exists(exists)
    {
        exists = true;
    }

    ~NotifyOnDeletejob()
    {
        m_exists = false;
    }

    void run(ThreadWeaver::JobPointer, ThreadWeaver::Thread *) {}

private:
    bool &m_exists;
};

class LifecycleTests : public QObject
{
    Q_OBJECT

public:
    LifecycleTests();

private Q_SLOTS:
    void testJobAutoDeletionBasics();
    void testJobAutoDeletion();
};

LifecycleTests::LifecycleTests()
{
}

void LifecycleTests::testJobAutoDeletionBasics()
{
    bool job1Exists = false;
    bool job2Exists = false;
    ThreadWeaver::JobPointer job2(new NotifyOnDeletejob(job2Exists));
    Q_UNUSED(job2);
    QCOMPARE(true, job2Exists);
    {
        ThreadWeaver::JobPointer job1(new NotifyOnDeletejob(job1Exists));
        Q_UNUSED(job1);
        QCOMPARE(job1Exists, true);
    }
    QCOMPARE(job1Exists, false);
    QCOMPARE(job2Exists, true);
}

void LifecycleTests::testJobAutoDeletion()
{
    bool job1Exists = false;
    bool job2Exists = false;
    {
        ThreadWeaver::JobPointer job1(new NotifyOnDeletejob(job1Exists));
        QCOMPARE(job1Exists, true);
        int argc = 0;
        QCoreApplication app(argc, (char **)0); Q_UNUSED(app);
        QVERIFY(ThreadWeaver::Queue::instance());
        ThreadWeaver::Queue::instance()->suspend();
        ThreadWeaver::Queue::instance()->enqueue(job1);
        ThreadWeaver::Queue::instance()->enqueue(ThreadWeaver::JobPointer(new NotifyOnDeletejob(job2Exists)));
        QCOMPARE(job2Exists, true);
        ThreadWeaver::Queue::instance()->resume();
        ThreadWeaver::Queue::instance()->finish();
        QVERIFY(ThreadWeaver::Queue::instance()->isIdle());
        ThreadWeaver::Queue::instance()->suspend();
        QEXPECT_FAIL("", "TODO this *should* work!", Continue);
        QCOMPARE(job2Exists, false);
        QCOMPARE(job1Exists, true);
    }
    QVERIFY(ThreadWeaver::Queue::instance() == 0);
    QCOMPARE(job2Exists, false);
    QCOMPARE(job1Exists, false);
}

QTEST_APPLESS_MAIN(LifecycleTests)

#include "LifecycleTests.moc"
