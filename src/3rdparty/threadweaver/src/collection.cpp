/* -*- C++ -*-

This file implements the Collection class.

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
*/

#include "collection.h"

#include "queueapi_p.h"
#include "debuggingaids.h"
#include "managedjobpointer.h"
#include "queueing.h"

#include <QtCore/QList>
#include <QtCore/QObject>
#include <QtCore/QPointer>

#include "dependencypolicy.h"
#include "executewrapper_p.h"
#include "thread.h"

namespace ThreadWeaver
{

class CollectionExecuteWrapper : public ExecuteWrapper
{
public:
    CollectionExecuteWrapper()
        : collection(0)
    {}

    void setCollection(Collection *collection_)
    {
        collection = collection_;
    }

    void begin(JobPointer job, Thread *thread) Q_DECL_OVERRIDE {
        ExecuteWrapper::begin(job, thread);
        Q_ASSERT(collection);
        collection->elementStarted(job, thread);
    }

    void end(JobPointer job, Thread *thread) Q_DECL_OVERRIDE {
        Q_ASSERT(collection);
        collection->elementFinished(job, thread);
        ExecuteWrapper::end(job, thread);
    }

    void cleanup(JobPointer job, Thread *) Q_DECL_OVERRIDE {
        //Once job is unwrapped from us, this object is dangling. Job::executor points to the next higher up execute wrapper.
        //It is thus safe to "delete this". By no means add any later steps after delete!
        delete unwrap(job);
    }

private:
    ThreadWeaver::Collection *collection;
};

class CollectionSelfExecuteWrapper : public ThreadWeaver::ExecuteWrapper
{
public:
    void begin(JobPointer, Thread *) Q_DECL_OVERRIDE {
    }

    void end(JobPointer, Thread *) Q_DECL_OVERRIDE {
    }
};

class Collection::Private
{
public:
    Private()
        : api(0)
        , jobCounter(0)
        , selfIsExecuting(false)
    {
    }

    ~Private()
    {
    }

    /* The elements of the collection. */
    QVector<JobPointer> elements;

    /* The Weaver interface this collection is queued in. */
    QueueAPI *api;

    /* Counter for the finished jobs.
       Set to the number of elements when started.
       When zero, all elements are done.
    */
    QAtomicInt jobCounter;
    QAtomicInt jobsStarted;
    CollectionSelfExecuteWrapper selfExecuteWrapper;
    JobPointer self;
    bool selfIsExecuting;
};

Collection::Collection()
    : d(new Private)
{
    d->selfExecuteWrapper.wrap(setExecutor(&d->selfExecuteWrapper));
    CollectionExecuteWrapper *wrapper = new CollectionExecuteWrapper();
    wrapper->setCollection(this);
    wrapper->wrap(setExecutor(wrapper));
}

Collection::~Collection()
{
    {
        // dequeue all remaining jobs:
        QMutexLocker l(mutex()); Q_UNUSED(l);
        if (d->api != 0) { // still queued
            dequeueElements(false);
        }
    }
    delete d;
}

void Collection::addJob(JobPointer job)
{
    QMutexLocker l(mutex()); Q_UNUSED(l);
    REQUIRE(d->api == 0 || d->selfIsExecuting == true); // not queued yet or still running
    REQUIRE(job != 0);

    CollectionExecuteWrapper *wrapper = new CollectionExecuteWrapper();
    wrapper->setCollection(this);
    wrapper->wrap(job->setExecutor(wrapper));
    d->elements.append(job);
}

void Collection::stop(JobPointer job)
{
    Q_UNUSED(job);
    QMutexLocker l(mutex()); Q_UNUSED(l);
    if (d->api != 0) {
        debug(4, "Collection::stop: dequeueing %p.\n", (void *)this);
        if (!d->api->dequeue(ManagedJobPointer<Collection>(this))) {
            dequeueElements(false);
        }
    }
}

void Collection::aboutToBeQueued_locked(QueueAPI *api)
{
    Q_ASSERT(!mutex()->tryLock());
    Q_ASSERT(d->api == 0); // never queue twice
    d->api = api;
    Job::aboutToBeQueued_locked(api);
}

void Collection::aboutToBeDequeued_locked(QueueAPI *api)
{
    Q_ASSERT(!mutex()->tryLock());
    Q_ASSERT(api && d->api == api);
    dequeueElements(true);
    d->api = 0;
    Job::aboutToBeDequeued_locked(api);
}

void Collection::execute(JobPointer job, Thread *thread)
{
    {
        QMutexLocker l(mutex()); Q_UNUSED(l);
        Q_ASSERT(d->self.isNull());
        Q_ASSERT(d->api != 0);
        d->self = job;
        d->selfIsExecuting = true; // reset in elementFinished
    }
    Job::execute(job, thread);
}

void Collection::run(JobPointer, Thread *)
{
    //empty
}

void Collection::enqueueElements()
{
    Q_ASSERT(!mutex()->tryLock());
    d->jobCounter.fetchAndStoreOrdered(d->elements.count() + 1); //including self
    d->api->enqueue(d->elements);
}

void Collection::elementStarted(JobPointer job, Thread *thread)
{
    Q_UNUSED(job) // except in Q_ASSERT
#ifndef NDEBUG // to avoid the mutex in release mode
    Q_ASSERT(!d->self.isNull());
    QMutexLocker l(mutex()); Q_UNUSED(l);
    Q_ASSERT(job.data() == d->self || std::find(d->elements.begin(), d->elements.end(), job) != d->elements.end());
#endif
    if (d->jobsStarted.fetchAndAddOrdered(1) == 0) {
        //emit started() signal on beginning of first job execution
        executor()->defaultBegin(d->self, thread);
    }
}

void Collection::elementFinished(JobPointer job, Thread *thread)
{
    QMutexLocker l(mutex()); Q_UNUSED(l);
    Q_ASSERT(!d->self.isNull());
    Q_UNUSED(job) // except in Q_ASSERT
    //FIXME test this assert with a decorated collection!
    Q_ASSERT(job.data() == d->self || std::find(d->elements.begin(), d->elements.end(), job) != d->elements.end());
    if (d->selfIsExecuting) {
        // the element that is finished is the collection itself
        // the collection is always executed first
        // queue the collection elements:
        enqueueElements();
        d->selfIsExecuting = false;
    }
    const int jobsStarted = d->jobsStarted.loadAcquire();
    Q_ASSERT(jobsStarted >= 0); Q_UNUSED(jobsStarted);
    const int remainingJobs = d->jobCounter.fetchAndAddOrdered(-1) - 1;
    Q_ASSERT(remainingJobs >= 0);
    if (remainingJobs == 0) {
        // all elements can only be done if self has been executed:
        // there is a small chance that (this) has been dequeued in the
        // meantime, in this case, there is nothing left to clean up
        finalCleanup();
        executor()->defaultEnd(d->self, thread);
        l.unlock();
        d->self.clear();
    }
}

JobPointer Collection::self() const
{
    return d->self;
}

JobPointer Collection::jobAt(int i)
{
    Q_ASSERT(!mutex()->tryLock());
    Q_ASSERT(i >= 0 && i < d->elements.size());
    return d->elements.at(i);
}

int Collection::jobListLength() const
{
    QMutexLocker l(mutex()); Q_UNUSED(l);
    return jobListLength_locked();
}

int Collection::jobListLength_locked() const
{
    return d->elements.size();
}

void Collection::finalCleanup()
{
    Q_ASSERT(!self().isNull());
    Q_ASSERT(!mutex()->tryLock());
    freeQueuePolicyResources(self());
    setStatus(Status_Success);
    d->api = 0;
}

Collection &Collection::operator<<(JobInterface *job)
{
    addJob(make_job(job));
    return *this;
}

Collection &Collection::operator<<(const JobPointer &job)
{
    addJob(job);
    return *this;
}

Collection &Collection::operator<<(JobInterface &job)
{
    addJob(make_job_raw(&job));
    return *this;
}

void Collection::dequeueElements(bool queueApiIsLocked)
{
    // dequeue everything:
    Q_ASSERT(!mutex()->tryLock());
    if (d->api == 0) {
        return;    //not queued
    }

    for (int index = 0; index < d->elements.size(); ++index) {
        debug(4, "Collection::dequeueElements: dequeueing %p.\n", (void *)d->elements.at(index).data());
        if (queueApiIsLocked) {
            d->api->dequeue_p(d->elements.at(index));
        } else {
            d->api->dequeue(d->elements.at(index));
        }
    }

    const int jobCount = d->jobCounter.fetchAndStoreAcquire(0);
    if (jobCount != 0) {
        // if jobCounter is not zero, then we where waiting for the
        // last job to finish before we would have freed our queue
        // policies. In this case we have to do it here:
        finalCleanup();
    }
}

}

