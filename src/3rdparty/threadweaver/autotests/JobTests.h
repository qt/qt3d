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

#ifndef JOBTESTS_H
#define JOBTESTS_H

#include <QtCore/QObject>
#include <ThreadWeaver/JobPointer>

namespace ThreadWeaver
{
class Job;
}

using ThreadWeaver::Job;

class JobTests : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void initTestCase();
    void WeaverLazyThreadCreationTest();
    void SimpleJobTest();
    void SimpleJobCollectionTest();
    void EmptyJobCollectionTest();
    void CollectionQueueingTest();
    void GeneratingCollectionTest();
    void ShortJobSequenceTest();
    void EmptyJobSequenceTest();
    void GeneratingSequenceTest();
    void IncompleteCollectionTest();
    void EmitStartedOnFirstElementTest();
    void CollectionDependenciesTest();
    void QueueAndDequeueCollectionTest();
    void QueueAndDequeueSequenceTest();
    void BlockingExecuteTest();
    void RecursiveSequenceTest();
    void RecursiveQueueAndDequeueCollectionTest();
    void RecursiveQueueAndDequeueSequenceTest();
    void QueueAndDequeueAllCollectionTest();
    void QueueAndDequeueAllSequenceTest();
    void RecursiveQueueAndDequeueAllCollectionTest();
    void RecursiveQueueAndDequeueAllSequenceTest();
    void MassiveJobSequenceTest();
    void SimpleRecursiveSequencesTest();
    void SequenceOfSequencesTest();
    void QueueAndStopTest();
    void ResourceRestrictionPolicyBasicsTest();
    void JobSignalsAreEmittedAsynchronouslyTest();
    void JobSignalsDeliveryTest();
    void JobPointerExecutionTest();
    void DequeueSuspendedSequenceTest();
    void IdDecoratorDecoratesTest();
    void IdDecoratorAutoDeleteTest();
    void IdDecoratorSingleAllocationTest();
    void JobsAreDestroyedAfterFinishTest();
    void JobExitStatusByExceptionTest();
    void JobManualExitStatusTest();
    void QueueStreamLifecycletest();

public Q_SLOTS: // slots used during tests that are not test cases
    void jobStarted(ThreadWeaver::JobPointer);
    void jobDone(ThreadWeaver::JobPointer);

    void deliveryTestJobDone(ThreadWeaver::JobPointer);
};

#endif
