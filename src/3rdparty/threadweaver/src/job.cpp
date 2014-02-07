/* -*- C++ -*-

This file implements the Job class.

$ Author: Mirko Boehm $
$ Copyright: (C) 2004-2013 Mirko Boehm $
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

$Id: Job.cpp 20 2005-08-08 21:02:51Z mirko $
*/

#include "job.h"

#include <QtCore/QList>
#include <QtCore/QMutex>
#include "debuggingaids.h"
#include "thread.h"
#include <QAtomicPointer>
#include <QAtomicInt>

#include "queuepolicy.h"
#include "dependencypolicy.h"
#include "executor_p.h"
#include "executewrapper_p.h"
#include "managedjobpointer.h"
#include "exception.h"

namespace
{

class DefaultExecutor : public ThreadWeaver::Executor
{
public:
    void begin(ThreadWeaver::JobPointer job, ThreadWeaver::Thread *thread)
    {
        defaultBegin(job, thread);
    }

    void execute(ThreadWeaver::JobPointer job, ThreadWeaver::Thread *thread) Q_DECL_OVERRIDE {
        run(job, thread);
    }

    void end(ThreadWeaver::JobPointer job, ThreadWeaver::Thread *thread)
    {
        defaultEnd(job, thread);
    }
};

class DebugExecuteWrapper : public ThreadWeaver::ExecuteWrapper
{
public:
    void execute(ThreadWeaver::JobPointer job, ThreadWeaver::Thread *th) Q_DECL_OVERRIDE {
        Q_ASSERT_X(job, Q_FUNC_INFO, "job may not be zero!");
        ThreadWeaver::debug(3, "DefaultExecuteWrapper::execute: executing job %p in thread %i.\n", job.data(), th ? th->id() : 0);
        executeWrapped(job, th);
        ThreadWeaver::debug(3, "Job::execute: finished execution of job in thread %i.\n", th ? th->id() : 0);
    }
};

static DefaultExecutor defaultExecutor;
}

namespace ThreadWeaver
{

class Job::Private
{
public:
    Private()
        : mutex(QMutex::NonRecursive)
        , status(Job::Status_NoStatus)
        , executor(&defaultExecutor)
    {}

    ~Private()
    {}

    /* The list of QueuePolicies assigned to this Job. */
    QList<QueuePolicy *> queuePolicies;

    mutable QMutex mutex;
    /* @brief The status of the Job. */
    QAtomicInt status;

    /** The Executor that will execute this Job. */
    QAtomicPointer<Executor> executor;

    //FIXME What is the correct KDE frameworks no debug switch?
#if !defined(NDEBUG)
    /** DebugExecuteWrapper for logging of Job execution. */
    DebugExecuteWrapper debugExecuteWrapper;
#endif
};

Job::Job()
    : d(new Private())
{
#if !defined(NDEBUG)
    d->debugExecuteWrapper.wrap(setExecutor(&d->debugExecuteWrapper));
#endif
    d->status.storeRelease(Status_New);
}

Job::~Job()
{
    for (int index = 0; index < d->queuePolicies.size(); ++index) {
        d->queuePolicies.at(index)->destructed(this);
    }
    delete d;
}

void Job::execute(JobPointer self, Thread *th)
{
    Executor *executor = d->executor.loadAcquire();
    Q_ASSERT(executor); //may never be unset!
    Q_ASSERT(self);
    executor->begin(self, th);
    self->setStatus(Status_Running);
    try {
        executor->execute(self, th);
        if (self->status() == Status_Running) {
            self->setStatus(Status_Success);
        }
    } catch (JobAborted &) {
        self->setStatus(Status_Aborted);
    } catch (JobFailed &) {
        self->setStatus(Status_Failed);
    } catch (...) {
        debug(0, "Uncaught exception in Job %p, aborting.", self.data());
        throw;
    }
    Q_ASSERT(self->status() > Status_Running);
    executor->end(self, th);
    executor->cleanup(self, th);
}

void Job::blockingExecute()
{
    execute(ManagedJobPointer<Job>(this), 0);
}
Executor *Job::setExecutor(Executor *executor)
{
    return d->executor.fetchAndStoreOrdered(executor == 0 ? &defaultExecutor : executor);
}

Executor *Job::executor() const
{
    return d->executor.fetchAndAddOrdered(0);
}

int Job::priority() const
{
    return 0;
}

void Job::setStatus(JobInterface::Status status)
{
    d->status.storeRelease(status);
}

JobInterface::Status Job::status() const
{
    // since status is set only through setStatus, this should be safe:
    return static_cast<Status>(d->status.loadAcquire());
}

bool Job::success() const
{
    return d->status.loadAcquire() == Status_Success;
}

void Job::freeQueuePolicyResources(JobPointer job)
{
    for (int index = 0; index < d->queuePolicies.size(); ++index) {
        d->queuePolicies.at(index)->free(job);
    }
}

void Job::defaultBegin(JobPointer, Thread *)
{
}

void Job::defaultEnd(JobPointer job, Thread *)
{
    freeQueuePolicyResources(job);
}

void Job::aboutToBeQueued(QueueAPI *api)
{
    QMutexLocker l(mutex()); Q_UNUSED(l);
    aboutToBeQueued_locked(api);
}

void Job::aboutToBeQueued_locked(QueueAPI *)
{
}

void Job::aboutToBeDequeued(QueueAPI *api)
{
    QMutexLocker l(mutex()); Q_UNUSED(l);
    aboutToBeDequeued_locked(api);
}

void Job::aboutToBeDequeued_locked(QueueAPI *)
{
}

void Job::assignQueuePolicy(QueuePolicy *policy)
{
    if (! d->queuePolicies.contains(policy)) {
        d->queuePolicies.append(policy);
    }
}

void Job::removeQueuePolicy(QueuePolicy *policy)
{
    int index = d->queuePolicies.indexOf(policy);
    if (index != -1) {
        d->queuePolicies.removeAt(index);
    }
}

QList<QueuePolicy *> Job::queuePolicies() const
{
    return d->queuePolicies;
}

bool Job::isFinished() const
{
    const Status s = status();
    return s == Status_Success || s == Status_Failed || s == Status_Aborted;
}

QMutex *Job::mutex() const
{
    return &d->mutex;
}

}

#include "managedjobpointer.h"
