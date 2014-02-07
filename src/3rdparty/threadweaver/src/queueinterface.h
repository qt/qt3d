/* -*- C++ -*-

   This file declares the QueueInterface class.

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

#ifndef WeaverInterface_H
#define WeaverInterface_H

#include <QtCore/QObject>
#include <QVector>

#include "threadweaver_export.h"
#include "jobpointer.h"
#include "jobinterface.h"

namespace ThreadWeaver
{

class Job;
class State;
class WeaverObserver;

/** WeaverInterface provides a common interface for weaver implementations.

In most cases, it is sufficient for an application to hold exactly one
ThreadWeaver job queue. To execute jobs in a specific order, use job
dependencies. To limit the number of jobs of a certain type that can
be executed at the same time, use resource restrictions. To handle
special requirements of the application when it comes to the order of
execution of jobs, implement a special queue policy and apply it to
the jobs.

Users of the ThreadWeaver API are encouraged to program to this
interface, instead of the implementation. This way, implementation
changes will not affect user programs.

This interface can be used for example to implement adapters and
decorators. The member documentation is provided in the Weaver and
WeaverImpl classes.
*/

class THREADWEAVER_EXPORT QueueInterface
{
public:
    virtual ~QueueInterface() {}
    /** Return the state of the weaver object. */
    virtual const State *state() const = 0;

    /** Shut down the queue.
     * Tells all threads to exit, and changes to Destructed state.
     * It is safe to destroy the queue once this method returns.
     */
    virtual void shutDown() = 0;

    /** Set the maximum number of threads this Weaver object may start. */
    virtual void setMaximumNumberOfThreads(int cap) = 0;

    /** Get the maximum number of threads this Weaver may start. */
    virtual int maximumNumberOfThreads() const = 0;

    /** Returns the current number of threads in the inventory. */
    virtual int currentNumberOfThreads() const = 0;

    /** Queue a vector of jobs.

    It depends on the state if execution of the job will be attempted
    immediately. In suspended state, jobs can be added to the queue,
    but the threads remain suspended. In WorkongHard state, an idle
    thread may immediately execute the job, or it might be queued if
    all threads are busy.

    JobPointer is a shared pointer. This means the object pointed to will be deleted if this object
    is the last remaining reference to it. Keep a JobPointer to the job to avoid automatic deletion.
    */
    virtual void enqueue(const QVector<JobPointer> &jobs) = 0;

    /** Remove a job from the queue.
     *
     * If the job was queued but not started so far, it is removed from the queue.
     *
     * You can always call dequeue, it will return true if the job was dequeued. However if the job is not in the queue anymore,
     * it is already being executed, it is too late to dequeue, and dequeue will return false. The return value is thread-safe - if
     * true is returned, the job was still waiting, and has been dequeued. If not, the job was not waiting in the queue.
     *
     * Modifying queued jobs is best done on a suspended queue. Often, for example at the end of an application, it is sufficient
     * to dequeue all jobs (which leaves only the ones mid-air in threads), call finish (that will wait for all the mid air jobs to
     * complete), and then exit. Without dequeue(), all jobs in the queue would be executed during finish().
     * @see requestAbort for aborting jobs during execution
     * @return true if the job was waiting and has been dequeued
     * @return false if the job was not found waiting in the queue
     */
    virtual bool dequeue(const JobPointer &job) = 0;

    /** Remove all queued jobs.
     *
     * All waiting jobs will be dequeued. The semantics are the same as for dequeue(JobInterface).
     *
     * @see dequeue(JobInterface)
     */
    virtual void dequeue() = 0;
    /** Finish all queued operations, then return.

    This method is used in imperative (not event driven) programs that
    cannot react on events to have the controlling (main) thread wait
    wait for the jobs to finish. The call will block the calling
    thread and return when all queued jobs have been processed.

    Warning: This will suspend your thread!
    Warning: If one of your jobs enters an infinite loop, this
    will never return! */
    virtual void finish() = 0;
    /** Suspend job execution.
    When suspending, all threads are allowed to finish the
    currently assigned job but will not receive a new
    assignment.
    When all threads are done processing the assigned job, the
    signal suspended will() be emitted.
    If you call suspend() and there are no jobs left to
    be done, you will immediately receive the suspended()
    signal. */
    virtual void suspend() = 0;
    /** Resume job queueing.
    @see suspend
    */
    virtual void resume() = 0;
    /** Is the queue empty?
    The queue is empty if no more jobs are queued. */
    virtual bool isEmpty() const = 0;
    /** Is the weaver idle?
    The weaver is idle if no jobs are queued and no jobs are processed
    by the threads. */
    virtual bool isIdle() const = 0;
    /** Returns the number of pending jobs.
    This will return the number of queued jobs. Jobs that are
    currently being executed are not part of the queue. All jobs in
    the queue are waiting to be executed.
    */
    virtual int queueLength() const = 0;

    /** Request aborts of the currently executed jobs.
    It is important to understand that aborts are requested, but
    cannot be guaranteed, as not all Job classes support it. It is up
    to the application to decide if and how job aborts are
    necessary. */
    virtual void requestAbort() = 0;

    /** @brief Reschedule the jobs in the queue.
     * This method triggers a scheduling attempt to perform jobs. It will schedule enqueued jobs to be executed by idle threads.
     * It should only be necessary to call it if the canRun() status of a job changed spontaneously due to external reasons. */
    virtual void reschedule() = 0;
};

}

#endif
