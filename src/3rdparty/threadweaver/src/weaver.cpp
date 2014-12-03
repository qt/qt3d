/* -*- C++ -*-

This file implements the WeaverImpl class.

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

$Id: WeaverImpl.cpp 30 2005-08-16 16:16:04Z mirko $

*/

#include "weaver_p.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QObject>
#include <QtCore/QMutex>
#include <QtCore/QDebug>

#include "job.h"
#include "managedjobpointer.h"
#include "state.h"
#include "thread.h"
#include "threadweaver.h"
#include "debuggingaids.h"
#include "suspendedstate_p.h"
#include "suspendingstate_p.h"
#include "destructedstate_p.h"
#include "workinghardstate_p.h"
#include "shuttingdownstate_p.h"
#include "inconstructionstate_p.h"
#include "queuepolicy.h"

using namespace ThreadWeaver;

/** @brief Constructs a Weaver object. */
Weaver::Weaver(QObject *parent)
    : QueueAPI(parent)
    , m_active(0)
    , m_inventoryMax(qMax(4, 2 * QThread::idealThreadCount()))
    , m_mutex(new QMutex(QMutex::NonRecursive))
{
    qRegisterMetaType<ThreadWeaver::JobPointer>("ThreadWeaver::JobPointer");

    QMutexLocker l(m_mutex); Q_UNUSED(l);
    // initialize state objects:
    m_states[InConstruction] = QSharedPointer<State>(new InConstructionState(this));
    setState_p(InConstruction);
    m_states[WorkingHard] = QSharedPointer<State>(new WorkingHardState(this));
    m_states[Suspending] = QSharedPointer<State>(new SuspendingState(this));
    m_states[Suspended] = QSharedPointer<State>(new SuspendedState(this));
    m_states[ShuttingDown] = QSharedPointer<State>(new ShuttingDownState(this));
    m_states[Destructed] = QSharedPointer<State>(new DestructedState(this));
    setState_p(WorkingHard);
}

/** @brief Destructs a Weaver object. */
Weaver::~Weaver()
{
    Q_ASSERT_X(state()->stateId() == Destructed, Q_FUNC_INFO, "shutDown() method was not called before Weaver destructor!");
    delete m_mutex;
}

/** @brief Enter Destructed state.
  *
  * Once this method returns, it is save to delete this object.
  */
void Weaver::shutDown()
{
    state()->shutDown();
}

void Weaver::shutDown_p()
{
    // the constructor may only be called from the thread that owns this
    // object (everything else would be what we professionals call "insane")

    REQUIRE(QThread::currentThread() == thread());
    debug(3, "WeaverImpl::shutDown: destroying inventory.\n");
    m_semaphore.acquire(m_createdThreads.loadAcquire());
    finish();
    suspend();
    setState(ShuttingDown);
    reschedule();
    m_jobFinished.wakeAll();

    // problem: Some threads might not be asleep yet, just finding
    // out if a job is available. Those threads will suspend
    // waiting for their next job (a rare case, but not impossible).
    // Therefore, if we encounter a thread that has not exited, we
    // have to wake it again (which we do in the following for
    // loop).

    for (;;) {
        Thread *th = 0;
        {
            QMutexLocker l(m_mutex); Q_UNUSED(l);
            if (m_inventory.isEmpty()) {
                break;
            }
            th = m_inventory.takeFirst();
        }
        if (!th->isFinished()) {
            for (;;) {
                Q_ASSERT(state()->stateId() == ShuttingDown);
                reschedule();
                if (th->wait(100)) {
                    break;
                }
                debug(1,  "WeaverImpl::shutDown: thread %i did not exit as expected, "
                      "retrying.\n", th->id());
            }
        }
        emit(threadExited(th));
        delete th;
    }
    Q_ASSERT(m_inventory.isEmpty());
    debug(3, "WeaverImpl::shutDown: done\n");
    setState(Destructed);    // Destructed ignores all calls into the queue API
}

/** @brief Set the Weaver state.
 * @see StateId
 * @see WeaverImplState
 * @see State
 */
void Weaver::setState(StateId id)
{
    QMutexLocker l(m_mutex); Q_UNUSED(l);
    setState_p(id);
}

void Weaver::setState_p(StateId id)
{
    Q_ASSERT(!m_mutex->tryLock()); //mutex has to be held when this method is called
    State *newState = m_states[id].data();
    State *previous = m_state.fetchAndStoreOrdered(newState);
    if (previous == 0 || previous->stateId() != id) {
        newState->activated();
        debug(2, "WeaverImpl::setState: state changed to \"%s\".\n", newState->stateName().toLatin1().constData());
        if (id == Suspended) {
            emit(suspended());
        }
        emit(stateChanged(newState));
    }
}

const State *Weaver::state() const
{
    return m_state.loadAcquire();
}

State *Weaver::state()
{
    return m_state.loadAcquire();
}

void Weaver::setMaximumNumberOfThreads(int cap)
{
    //FIXME really? Why not 0?
    Q_ASSERT_X(cap > 0, "Weaver Impl", "Thread inventory size has to be larger than zero.");
    QMutexLocker l(m_mutex);  Q_UNUSED(l);
    state()->setMaximumNumberOfThreads(cap);
}

void Weaver::setMaximumNumberOfThreads_p(int cap)
{
    Q_ASSERT(!m_mutex->tryLock()); //mutex has to be held when this method is called
    m_inventoryMax = cap;
}

int Weaver::maximumNumberOfThreads() const
{
    QMutexLocker l(m_mutex); Q_UNUSED(l);
    return state()->maximumNumberOfThreads();
}

int Weaver::maximumNumberOfThreads_p() const
{
    Q_ASSERT(!m_mutex->tryLock()); //mutex has to be held when this method is called
    return m_inventoryMax;
}

int Weaver::currentNumberOfThreads() const
{
    QMutexLocker l(m_mutex); Q_UNUSED(l);
    return state()->currentNumberOfThreads();
}

int Weaver::currentNumberOfThreads_p() const
{
    Q_ASSERT(!m_mutex->tryLock()); //mutex has to be held when this method is called
    return m_inventory.count();
}

void Weaver::enqueue(const QVector<JobPointer> &jobs)
{
    QMutexLocker l(m_mutex); Q_UNUSED(l);
    state()->enqueue(jobs);
}

void Weaver::enqueue_p(const QVector<JobPointer> &jobs)
{
    Q_ASSERT(!m_mutex->tryLock()); //mutex has to be held when this method is called
    if (jobs.isEmpty()) {
        return;
    }
    Q_FOREACH (const JobPointer &job, jobs) {
        if (job) {
            Q_ASSERT(job->status() == Job::Status_New);
            adjustInventory(1);
            debug(3, "WeaverImpl::enqueue: queueing job %p.\n", (void *)job.data());
            job->aboutToBeQueued(this);
            // find position for insertion:
            int i = m_assignments.size();
            if (i > 0) {
                while (i > 0 && m_assignments.at(i - 1)->priority() < job->priority()) {
                    --i;
                }
                m_assignments.insert(i, job);
            } else {
                m_assignments.append(job);
            }
            job->setStatus(Job::Status_Queued);
            reschedule();
        }
    }
}

bool Weaver::dequeue(const JobPointer &job)
{
    QMutexLocker l(m_mutex); Q_UNUSED(l);
    return state()->dequeue(job);
}

bool Weaver::dequeue_p(JobPointer job)
{
    Q_ASSERT(!m_mutex->tryLock()); //mutex has to be held when this method is called
    int position = m_assignments.indexOf(job);
    if (position != -1) {
        job->aboutToBeDequeued(this);
        int newPosition = m_assignments.indexOf(job);
        JobPointer job = m_assignments.takeAt(newPosition);
        job->setStatus(Job::Status_New);
        Q_ASSERT(!m_assignments.contains(job));
        debug(3, "WeaverImpl::dequeue: job %p dequeued, %i jobs left.\n", (void *)job.data(), queueLength_p());
        // from the queues point of view, a job is just as finished if it gets dequeued:
        m_jobFinished.wakeAll();
        Q_ASSERT(!m_assignments.contains(job));
        return true;
    } else {
        debug(3, "WeaverImpl::dequeue: job %p not found in queue.\n", (void *)job.data());
        return false;
    }
}

void Weaver::dequeue()
{
    QMutexLocker l(m_mutex); Q_UNUSED(l);
    state()->dequeue();
}

void Weaver::dequeue_p()
{
    Q_ASSERT(!m_mutex->tryLock()); //mutex has to be held when this method is called
    debug(3, "WeaverImpl::dequeue: dequeueing all jobs.\n");
    for (int index = 0; index < m_assignments.size(); ++index) {
        m_assignments.at(index)->aboutToBeDequeued(this);
    }
    m_assignments.clear();
    ENSURE(m_assignments.isEmpty());
}

void Weaver::finish()
{
    QMutexLocker l(m_mutex); Q_UNUSED(l);
    state()->finish();
}

void Weaver::finish_p()
{
    Q_ASSERT(!m_mutex->tryLock()); //mutex has to be held when this method is called
#ifdef QT_NO_DEBUG
    const int MaxWaitMilliSeconds = 50;
#else
    const int MaxWaitMilliSeconds = 500;
#endif
    while (!isIdle_p()) {
        Q_ASSERT_X(state()->stateId() == WorkingHard, Q_FUNC_INFO, qPrintable(state()->stateName()));
        debug(2, "WeaverImpl::finish: not done, waiting.\n");
        if (m_jobFinished.wait(m_mutex, MaxWaitMilliSeconds) == false) {
            debug(2, "WeaverImpl::finish: wait timed out, %i jobs left, waking threads.\n", queueLength_p());
            reschedule();
        }
    }
    debug(2, "WeaverImpl::finish: done.\n\n\n");
}

void Weaver::suspend()
{
    //FIXME?
    //QMutexLocker l(m_mutex); Q_UNUSED(l);
    state()->suspend();
}

void Weaver::suspend_p()
{
    //FIXME ?
}

void Weaver::resume()
{
    //FIXME?
    //QMutexLocker l(m_mutex); Q_UNUSED(l);
    state()->resume();
}

void Weaver::resume_p()
{
    //FIXME ?
}

bool Weaver::isEmpty() const
{
    QMutexLocker l(m_mutex); Q_UNUSED(l);
    return state()->isEmpty();
}

bool Weaver::isEmpty_p() const
{
    Q_ASSERT(!m_mutex->tryLock()); //mutex has to be held when this method is called
    return  m_assignments.isEmpty();
}

bool Weaver::isIdle() const
{
    QMutexLocker l(m_mutex); Q_UNUSED(l);
    return state()->isIdle();
}

bool Weaver::isIdle_p() const
{
    Q_ASSERT(!m_mutex->tryLock()); //mutex has to be held when this method is called
    return isEmpty_p() && m_active == 0;
}

int Weaver::queueLength() const
{
    QMutexLocker l(m_mutex); Q_UNUSED(l);
    return state()->queueLength();
}

int Weaver::queueLength_p() const
{
    Q_ASSERT(!m_mutex->tryLock()); //mutex has to be held when this method is called
    return m_assignments.count();
}

void Weaver::requestAbort()
{
    QMutexLocker l(m_mutex); Q_UNUSED(l);
    return state()->requestAbort();
}

void Weaver::reschedule()
{
    m_jobAvailable.wakeAll();
}

void Weaver::requestAbort_p()
{
    Q_ASSERT(!m_mutex->tryLock()); //mutex has to be held when this method is called
    for (int i = 0; i < m_inventory.size(); ++i) {
        m_inventory[i]->requestAbort();
    }
}

/** @brief Adjust the inventory size.
 *
 * Requires that the mutex is being held when called.
 *
 * This method creates threads on demand. Threads in the inventory
 * are not created upon construction of the WeaverImpl object, but
 * when jobs are queued. This avoids costly delays on the application
 * startup time. Threads are created when the inventory size is under
 * inventoryMin and new jobs are queued.
 */
//TODO add code to raise inventory size over inventoryMin
//TODO add code to quit unnecessary threads
void Weaver::adjustInventory(int numberOfNewJobs)
{
    Q_ASSERT(!m_mutex->tryLock()); //mutex has to be held when this method is called
    //number of threads that can be created:
    const int reserve = m_inventoryMax - m_inventory.count();

    if (reserve > 0) {
        for (int i = 0; i < qMin(reserve, numberOfNewJobs); ++i) {
            Thread *th = createThread();
            th->moveToThread(th);   // be sane from the start
            m_inventory.append(th);
            connect(th, SIGNAL(jobStarted(ThreadWeaver::JobPointer,ThreadWeaver::Thread*)),
                    SIGNAL(threadBusy(ThreadWeaver::JobPointer,ThreadWeaver::Thread*)));
            connect(th, SIGNAL(jobDone(ThreadWeaver::JobPointer)),
                    SIGNAL(jobDone(ThreadWeaver::JobPointer)));
            th->start();
            m_createdThreads.ref();
            debug(2, "WeaverImpl::adjustInventory: thread created, "
                  "%i threads in inventory.\n", currentNumberOfThreads_p());
        }
    }
}

/** @brief Check with the assigned queue policies if the job can be executed.
 *
 * If it returns true, it expects that the job is executed right after that. The done() methods of the
 * queue policies will be automatically called when the job is finished.
 *
 * If it returns false, all queue policy resources have been freed, and the method can be called again
 * at a later time.
 */
bool Weaver::canBeExecuted(JobPointer job)
{
    Q_ASSERT(!m_mutex->tryLock()); //mutex has to be held when this method is called

    QList<QueuePolicy *> acquired;

    bool success = true;

    QList<QueuePolicy *> policies = job->queuePolicies();
    if (!policies.isEmpty()) {
        debug(4, "WeaverImpl::canBeExecuted: acquiring permission from %i queue %s.\n",
              policies.size(), policies.size() == 1 ? "policy" : "policies");
        for (int index = 0; index < policies.size(); ++index) {
            if (policies.at(index)->canRun(job)) {
                acquired.append(policies.at(index));
            } else {
                success = false;
                break;
            }
        }

        debug(4, "WeaverImpl::canBeExecuted: queue policies returned %s.\n", success ? "true" : "false");

        if (!success) {
            for (int index = 0; index < acquired.size(); ++index) {
                acquired.at(index)->release(job);
            }
        }
    } else {
        debug(4, "WeaverImpl::canBeExecuted: no queue policies, this job can be executed.\n");
    }
    return success;
}

/** @brief Factory method to create the threads.
 *
 * Overload in adapted Weaver implementations.
 */
Thread *Weaver::createThread()
{
    return new Thread(this);
}

/** @brief Increment the count of active threads. */
void Weaver::incActiveThreadCount()
{
    adjustActiveThreadCount(1);
}

/** brief Decrement the count of active threads. */
void Weaver::decActiveThreadCount()
{
    adjustActiveThreadCount(-1);
    // the done job could have freed another set of jobs, and we do not know how
    // many - therefore we need to wake all threads:
    m_jobFinished.wakeAll();
}

/** @brief Adjust active thread count.
 *
 * This is a helper function for incActiveThreadCount and decActiveThreadCount.
 */
void Weaver::adjustActiveThreadCount(int diff)
{
    Q_ASSERT(!m_mutex->tryLock()); //mutex has to be held when this method is called
    m_active += diff;
    debug(4, "WeaverImpl::adjustActiveThreadCount: %i active threads (%i jobs"
          " in queue).\n", m_active,  queueLength_p());

    if (m_assignments.isEmpty() && m_active == 0) {
        P_ASSERT(diff < 0);    // cannot reach zero otherwise
        emit(finished());
    }
}

/** @brief Returns the number of active threads.
 *
 * Threads are active if they process a job. Requires that the mutex is being held when called.
 */
int Weaver::activeThreadCount()
{
    Q_ASSERT(!m_mutex->tryLock()); //mutex has to be held when this method is called
    return m_active;
}

/** @brief Called from a new thread when entering the run method. */
void Weaver::threadEnteredRun(Thread *thread)
{
    m_semaphore.release(1);
    emit threadStarted(thread);
}

/** @brief Take the first available job out of the queue and return it.
 *
 * The job will be removed from the queue (therefore, take). Only jobs that have no unresolved dependencies
 * are considered available. If only jobs that depened on other unfinished jobs are in the queue, this method
 * blocks on m_jobAvailable.
 *
 * This method will enter suspended state if the active thread count is now zero and
 * suspendIfAllThreadsInactive is true.
 * If justReturning is true, do not assign a new job, just process the completed previous one.
 */
JobPointer Weaver::takeFirstAvailableJobOrSuspendOrWait(Thread *th, bool threadWasBusy,
        bool suspendIfInactive, bool justReturning)
{
    QMutexLocker l(m_mutex); Q_UNUSED(l);
    Q_ASSERT(threadWasBusy == false || (threadWasBusy == true && m_active > 0));

#ifndef QT_NO_DEBUG
    debug(3, "WeaverImpl::takeFirstAvailableJobOrWait: trying to assign new job to thread %i (%s state).\n",
          th->id(), qPrintable(state()->stateName()));
    debug(5, "WeaverImpl::takeFirstAvailableJobOrWait: %i active threads, was busy: %s, suspend: %s, assign new job: %s.\n",
          activeThreadCount(), threadWasBusy ? "yes" : "no", suspendIfInactive ? "yes" : "no", !justReturning ? "yes" : "no");
#endif

    if (threadWasBusy) {
        // cleanup and send events:
        decActiveThreadCount();
    }
    Q_ASSERT(m_active >= 0);

    if (suspendIfInactive && m_active == 0 && state()->stateId() == Suspending) {
        setState_p(Suspended);
        return JobPointer();
    }

    if (state()->stateId() != WorkingHard || justReturning) {
        return JobPointer();
    }

    JobPointer next;
    for (int index = 0; index < m_assignments.size(); ++index) {
        const JobPointer &candidate = m_assignments.at(index);
        if (canBeExecuted(candidate)) {
            next = candidate;
            m_assignments.removeAt(index);
            break;
        }
    }
    if (next) {
        incActiveThreadCount();
        debug(3, "WeaverImpl::takeFirstAvailableJobOrWait: job %p assigned to thread %i (%s state).\n",
              next.data(), th->id(), qPrintable(state()->stateName()));
        return next;
    }

    blockThreadUntilJobsAreBeingAssigned_locked(th);
    return JobPointer();
}

/** @brief Assign a job to the calling thread.
 *
 * This is supposed to be called from the Thread objects in the inventory. Do not call this method from
 * your code.
 * Returns 0 if the weaver is shutting down, telling the calling thread to finish and exit. If no jobs are
 * available and shut down is not in progress, the calling thread is suspended until either condition is met.
 * @param wasBusy True if the thread is returning from processing a job
 */
JobPointer Weaver::applyForWork(Thread *th, bool wasBusy)
{
    return state()->applyForWork(th, wasBusy);
}

/** @brief Wait for a job to become available. */
void Weaver::waitForAvailableJob(Thread *th)
{
    state()->waitForAvailableJob(th);
}

/** @brief Blocks the calling thread until jobs can be assigned. */
void Weaver::blockThreadUntilJobsAreBeingAssigned(Thread *th)
{
    QMutexLocker l(m_mutex); Q_UNUSED(l);
    blockThreadUntilJobsAreBeingAssigned_locked(th);
}

/** @brief Blocks the calling thread until jobs can be assigned.
 *
 * The mutex must be held when calling this method.
 */
void Weaver::blockThreadUntilJobsAreBeingAssigned_locked(Thread *th)
{
    Q_ASSERT(!m_mutex->tryLock()); //mutex has to be held when this method is called
    debug(4, "WeaverImpl::blockThreadUntilJobsAreBeingAssigned_locked: thread %i blocked (%s state).\n",
          th->id(), qPrintable(state()->stateName()));
    emit threadSuspended(th);
    m_jobAvailable.wait(m_mutex);
    debug(4, "WeaverImpl::blockThreadUntilJobsAreBeingAssigned_locked: thread %i resumed  (%s state).\n",
          th->id(), qPrintable(state()->stateName()));
}

/** @brief Dump the current jobs to the console.
 *
 * Use at your own risk.
 */
void Weaver::dumpJobs()
{
    QMutexLocker l(m_mutex); Q_UNUSED(l);
    debug(0, "WeaverImpl::dumpJobs: current jobs:\n");
    for (int index = 0; index < m_assignments.size(); ++index) {
        debug(0, "--> %4i: %p (priority %i, can be executed: %s)\n", index, (void *)m_assignments.at(index).data(),
              m_assignments.at(index)->priority(),
              canBeExecuted(m_assignments.at(index)) ? "yes" : "no");
    }
}

