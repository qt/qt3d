/* -*- C++ -*-

This file implements the public interfaces of the WeaverImpl class.

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

$Id: WeaverImpl.h 32 2005-08-17 08:38:01Z mirko $
*/
#ifndef WeaverImpl_H
#define WeaverImpl_H

#include <QtCore/QObject>
#include <QtCore/QWaitCondition>
#include <QSharedPointer>
#include <QAtomicPointer>
#include <QAtomicInt>
#include <QSemaphore>
#include <QVector>

#include "state.h"
#include "queueapi_p.h"

namespace ThreadWeaver
{

class Job;
class Thread;
class WeaverImplState;
class SuspendingState;

/** @brief A Weaver manages worker threads.
 *
 * It creates an inventory of Thread objects to which it assigns jobs from its queue.
 * It extends the API of Queue, hiding methods that need to be public to implement state handling, but
 * should not be exposed in general.
 */
class THREADWEAVER_EXPORT Weaver : public QueueAPI
{
    Q_OBJECT
public:
    explicit Weaver(QObject *parent = 0);
    virtual ~Weaver();
    void shutDown() Q_DECL_OVERRIDE;
    void shutDown_p() Q_DECL_OVERRIDE;

    const State *state() const Q_DECL_OVERRIDE;
    State *state() Q_DECL_OVERRIDE;

    void setMaximumNumberOfThreads(int cap) Q_DECL_OVERRIDE;
    int maximumNumberOfThreads() const Q_DECL_OVERRIDE;
    int currentNumberOfThreads() const Q_DECL_OVERRIDE;

    void setState(StateId);
    void enqueue(const QVector<JobPointer> &jobs) Q_DECL_OVERRIDE;
    bool dequeue(const JobPointer &job) Q_DECL_OVERRIDE;
    void dequeue() Q_DECL_OVERRIDE;
    void finish() Q_DECL_OVERRIDE;
    void suspend() Q_DECL_OVERRIDE;
    void resume() Q_DECL_OVERRIDE;
    bool isEmpty() const Q_DECL_OVERRIDE;
    bool isIdle() const Q_DECL_OVERRIDE;
    int queueLength() const Q_DECL_OVERRIDE;
    virtual JobPointer applyForWork(Thread *thread, bool wasBusy) Q_DECL_OVERRIDE;
    void waitForAvailableJob(Thread *th) Q_DECL_OVERRIDE;
    void blockThreadUntilJobsAreBeingAssigned(Thread *th);
    void blockThreadUntilJobsAreBeingAssigned_locked(Thread *th);
    void incActiveThreadCount();
    void decActiveThreadCount();
    int activeThreadCount();

    void threadEnteredRun(Thread *thread);
    JobPointer takeFirstAvailableJobOrSuspendOrWait(Thread *th, bool threadWasBusy,
                                                    bool suspendIfAllThreadsInactive, bool justReturning);
    void requestAbort() Q_DECL_OVERRIDE;
    void reschedule() Q_DECL_OVERRIDE;

    void dumpJobs();

    //FIXME: rename _p to _locked:
    friend class WeaverImplState;
    friend class SuspendingState;
    void setState_p(StateId);
    void setMaximumNumberOfThreads_p(int cap) Q_DECL_OVERRIDE;
    int maximumNumberOfThreads_p() const Q_DECL_OVERRIDE;
    int currentNumberOfThreads_p() const Q_DECL_OVERRIDE;
    void enqueue_p(const QVector<JobPointer> &jobs);
    bool dequeue_p(JobPointer job) Q_DECL_OVERRIDE;
    void dequeue_p() Q_DECL_OVERRIDE;
    void finish_p() Q_DECL_OVERRIDE;
    void suspend_p() Q_DECL_OVERRIDE;
    void resume_p() Q_DECL_OVERRIDE;
    bool isEmpty_p() const Q_DECL_OVERRIDE;
    bool isIdle_p() const Q_DECL_OVERRIDE;
    int queueLength_p() const Q_DECL_OVERRIDE;
    void requestAbort_p() Q_DECL_OVERRIDE;

Q_SIGNALS:
    /** @brief A Thread has been created. */
    void threadStarted(ThreadWeaver::Thread *);
    /** @brief A thread has exited. */
    void threadExited(ThreadWeaver::Thread *);
    /** @brief A thread has been suspended. */
    void threadSuspended(ThreadWeaver::Thread *);
    /** @brief The thread is busy executing job j. */
    void threadBusy(ThreadWeaver::JobPointer, ThreadWeaver::Thread *);

protected:
    void adjustActiveThreadCount(int diff);
    virtual Thread *createThread();
    void adjustInventory(int noOfNewJobs);

private:
    bool canBeExecuted(JobPointer);
    /** The thread inventory. */
    QList<Thread *> m_inventory;
    /** The job queue. */
    QList<JobPointer> m_assignments;
    /** The number of jobs that are assigned to the worker threads, but not finished. */
    int m_active;
    /** The maximum number of worker threads. */
    int m_inventoryMax;
    /** Wait condition all idle or done threads wait for. */
    QWaitCondition m_jobAvailable;
    /** Wait for a job to finish. */
    QWaitCondition m_jobFinished;
    /** Mutex to serialize operations. */
    QMutex *m_mutex;
    /** Semaphore to ensure thread startup is in sequence. */
    QSemaphore m_semaphore;
    /** Before shutdown can proceed to close the running threads, it needs to ensure that all of them
     *  entered the run method. */
    QAtomicInt m_createdThreads;
    /** The state of the art.
    * @see StateId
    */
    QAtomicPointer<State> m_state;
    /** The state objects. */
    QSharedPointer<State> m_states[NoOfStates];
};

} // namespace ThreadWeaver

#endif // WeaverImpl_H
