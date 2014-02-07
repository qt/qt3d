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

#ifndef QUEUETESTS_H
#define QUEUETESTS_H

#include <QtCore/QThread>

#include "AppendCharacterJob.h"

class LowPriorityAppendCharacterJob : public AppendCharacterJob
{
public:
    LowPriorityAppendCharacterJob(QChar character = QChar(), QString *stringref = 0);

    int priority() const;
};

class HighPriorityAppendCharacterJob : public AppendCharacterJob
{
public:
    HighPriorityAppendCharacterJob(QChar character = QChar(), QString *stringref = 0);

    int priority() const;
};

namespace ThreadWeaver
{
class Job;
class Collection;
class QObjectDecorator;
}

using ThreadWeaver::Job;

class SecondThreadThatQueues : public QThread
{
    Q_OBJECT

public:
    SecondThreadThatQueues();

protected:
    void run();
};

class QueueTests : public QObject
{
    Q_OBJECT

public:
    explicit QueueTests(QObject *parent = 0);

public Q_SLOTS:
    // this slot (which is not a test) is part of
    // DeleteDoneJobsFromSequenceTest
    void deleteJob(ThreadWeaver::JobPointer);
    // this slot is part of DeleteCollectionOnDoneTest
    void deleteCollection(ThreadWeaver::JobPointer);

private:
    // this is also part of DeleteDoneJobsFromSequenceTest
    ThreadWeaver::QObjectDecorator *autoDeleteJob;
    // this is part of DeleteCollectionOnDoneTest
    ThreadWeaver::QObjectDecorator *autoDeleteCollection;

private Q_SLOTS:

    void initTestCase();

    void SimpleQueuePrioritiesTest();
    void WeaverInitializationTest();
    void QueueFromSecondThreadTest();
    void DeleteDoneJobsFromSequenceTest();
    void DeleteCollectionOnDoneTest();
};

#endif
