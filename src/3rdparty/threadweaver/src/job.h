/* -*- C++ -*-

This file declares the Job class.

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

$Id: Job.h 32 2005-08-17 08:38:01Z mirko $
*/

#ifndef THREADWEAVER_JOB_H
#define THREADWEAVER_JOB_H

#include "threadweaver_export.h"
#include "jobinterface.h"
#include "jobpointer.h"

class QMutex;
class QWaitCondition;

namespace ThreadWeaver
{

class Thread;
class QueuePolicy;
class QueueAPI;
class Executor;

/** @brief A Job is a simple abstraction of an action that is to be executed in a thread context.
 *
 * It is essential for the ThreadWeaver library that as a kind of convention, the different creators of Job objects do not touch
 * the protected data members of the Job until somehow notified by the Job.
 *
 * Jobs may not be executed twice. Create two different objects to perform two consecutive or parallel runs.
 * (Note: this rule is being reconsidered.)
 *
 * Jobs may declare dependencies. If Job B depends on Job A, B may not be executed before A is finished. To learn about
 * dependencies, see DependencyPolicy.
 *
 * Job objects do not inherit QObject. To connect to signals when jobs are started or finished, see QObjectDecorator.
 */
class THREADWEAVER_EXPORT Job : public JobInterface
{
public:
    /** Construct a Job. */
    Job();

    /** Destructor. */
    virtual ~Job();

    /** Perform the job. The thread in which this job is executed is given as a parameter.
     *
     * Do not overload this method to create your own Job implementation, overload run().
     * Whenever the currently executed job is communicated to the outside world, use the supplied job pointer
     * to keep the reference count correct.
     *
     * job is the Job that the queue is executing. It is not necessarily equal to this. For example, Jobs that are
     * decorated expose the decorator's address, not the address of the decorated object.
     */
    virtual void execute(JobPointer job, Thread *) Q_DECL_OVERRIDE;

    /** Perform the job synchroneously in the current thread. */
    void blockingExecute() Q_DECL_OVERRIDE;

    /** Set the Executor object that is supposed to run the job.
     *
     * Returns the previously set executor. The executor can never be unset. If zero is passed in as the new executor, the Job
     * will internally reset to a default executor that only invokes run().
     */
    Executor *setExecutor(Executor *executor) Q_DECL_OVERRIDE;

    /** Returns the executor currently set on the Job. */
    Executor *executor() const Q_DECL_OVERRIDE;

    /** The queueing priority of the job.
     * Jobs will be sorted by their queueing priority when enqueued. A higher queueing priority will place the job in front of all
     * lower-priority jobs in the queue.
     *
     * Note: A higher or lower priority does not influence queue policies. For example, a high-priority job that has an unresolved
     * dependency will not be executed, which means an available lower-priority job will take precedence.
     *
     * The default implementation returns zero. Only if this method is overloaded for some job classes, priorities will influence
     * the execution order of jobs. */
    int priority() const Q_DECL_OVERRIDE;

    /** @brief Set the status of the Job.
     *
     * Do not call this method unless you know what you are doing, please :-) */
    void setStatus(Status) Q_DECL_OVERRIDE;

    /** @brief The status of the job.
     *
     * The status will be changed to Status_Success if the run() method exits normally.
     */
    Status status() const Q_DECL_OVERRIDE;

    /** Return whether the Job finished successfully or not.
     * The default implementation simply returns true. Overload in derived classes if the derived Job class can fail.
     *
     * If a job fails (success() returns false), it will *NOT* resolve its dependencies when it finishes. This will make sure that
     * Jobs that depend on the failed job will not be started.
     *
     * There is an important gotcha: When a Job object it deleted, it will always resolve its dependencies. If dependent jobs should
     * not be executed after a failure, it is important to dequeue those before deleting the failed Job. A Sequence may be
     * helpful for that purpose.
     */
    bool success() const Q_DECL_OVERRIDE;

    /** Abort the execution of the job.
     *
     * Call this method to ask the Job to abort if it is currently executed. Please note that the default implementation of
     * the method does nothing (!). This is due to the fact that there is no generic method to abort a processing Job. Not even a
     * default boolean flag makes sense, as Job could, for example, be in an event loop and will need to create an exit event. You
     * have to reimplement the method to actually initiate an abort action.
     *
     * The method is not pure virtual because users are not supposed to be forced to always implement requestAbort(). Also, this
     * method is supposed to return immediately, not after the abort has completed. It requests the abort, the Job has to act on
     * the request. */
    void requestAbort() Q_DECL_OVERRIDE {}

    /** The job is about to be added to the weaver's job queue.
     *
     * The job will be added right after this method finished. The default implementation does nothing. Use this method to, for
     * example, queue sub-operations as jobs before the job itself is queued.
     *
     * Note: When this method is called, the associated Weaver object's thread holds a lock on the weaver's queue. Therefore, it
     * is save to assume that recursive queueing is atomic from the queues perspective.
     *
     * @param api the QueueAPI object the job will be queued in */
    void aboutToBeQueued(QueueAPI *api) Q_DECL_OVERRIDE;

    /** Called from aboutToBeQueued() while the mutex is being held. */
    void aboutToBeQueued_locked(QueueAPI *api) Q_DECL_OVERRIDE;

    /** This Job is about the be dequeued from the weaver's job queue.
     *
     * The job will be removed from the queue right after this method returns. Use this method to dequeue, if necessary,
     * sub-operations (jobs) that this job has enqueued.
     *
     * Note: When this method is called, the associated Weaver object's thread does hold a lock on the weaver's queue.
     * Note: The default implementation does nothing.
     *
     * @param weaver the Weaver object from which the job will be dequeued */
    void aboutToBeDequeued(QueueAPI *api) Q_DECL_OVERRIDE;

    /** Called from aboutToBeDequeued() while the mutex is being held. */
    void aboutToBeDequeued_locked(QueueAPI *api) Q_DECL_OVERRIDE;

    /** Returns true if the jobs's execute method finished. */
    bool isFinished() const Q_DECL_OVERRIDE;

    /** Assign a queue policy.
     *
     * Queue Policies customize the queueing (running) behaviour of sets of jobs. Examples for queue policies are dependencies
     * and resource restrictions. Every queue policy object can only be assigned once to a job, multiple assignments will be
     * IGNORED. */
    void assignQueuePolicy(QueuePolicy *) Q_DECL_OVERRIDE;

    /** Remove a queue policy from this job. */
    void removeQueuePolicy(QueuePolicy *) Q_DECL_OVERRIDE;

    /** @brief Return the queue policies assigned to this Job. */
    QList<QueuePolicy *> queuePolicies() const;

private:
    class Private;
    Private *d;

protected:
    /** Free the queue policies acquired before this job has been executed. */
    void freeQueuePolicyResources(JobPointer) Q_DECL_OVERRIDE;

    friend class Executor;
    /** The method that actually performs the job.
     *
     * It is called from execute(). This method is the one to overload it with the job's task.
     *
     * The Job will be executed in the specified thread. thread may be zero, indicating that the job is being executed some
     * other way (for example, synchroneously by some other job). self specifies the job as the queue sees it. Whenever publishing
     * information about the job to the outside world, for example by emitting signals, use self, not this. self is the reference
     * counted object handled by the queue. Using it as signal parameters will amongst other things prevent thejob from being
     * memory managed and deleted.
     */
    virtual void run(JobPointer self, Thread *thread) = 0;

    /** @brief Perform standard tasks before starting the execution of a job.
     *
     * The default implementation is empty.
     * job is the Job that the queue is executing. It is not necessarily equal to this. For example, Jobs that are
     * decorated expose the decorator's address, not the address of the decorated object. */
    void defaultBegin(JobPointer job, Thread *thread) Q_DECL_OVERRIDE;

    /** @brief Perform standard task after the execution of a job.
     *
     * The default implementation is empty.
     * job is the Job that the queue is executing. It is not necessarily equal to this. For example, Jobs that are
     * decorated expose the decorator's address, not the address of the decorated object. */
    void defaultEnd(JobPointer job, Thread *thread) Q_DECL_OVERRIDE;

    /** The mutex used to protect this job. */
    QMutex *mutex() const Q_DECL_OVERRIDE;

};

}

#endif // THREADWEAVER_JOB_H
