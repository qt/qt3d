/* -*- C++ -*-

   This file declares the Collection class.

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

#ifndef JOBCOLLECTION_H
#define JOBCOLLECTION_H

#include "job.h"
#include "jobpointer.h"

namespace ThreadWeaver
{

class Thread;
class CollectionExecuteWrapper;

/** A Collection is a vector of Jobs that will be queued together.
 * In a Collection, the order of execution of the elements is not specified.
 *
 * It is intended that the collection is set up first and then
 * queued. After queuing, no further jobs should be added to the collection.
 *
 * Collection emits a done(JobPointer) signal when all of the jobs in the collection
 * have completed.
 */
class THREADWEAVER_EXPORT Collection : public Job
{
public:
    Collection();
    ~Collection();
    /** Append a job to the collection.
     *
     * To use Collection, create the Job objects first, add them to the collection, and then queue it. After
     * the collection has been queued, no further Jobs are supposed to be added.
     *
     * @note Once the job has been added, execute wrappers can no more be set on it */
    virtual void addJob(JobPointer);

    /** Stop processing, dequeue all remaining Jobs.
     * job is supposed to be an element of the collection.
     */
    //FIXME remove job argument?
    void stop(ThreadWeaver::JobPointer job);

    /** Return the number of jobs in the joblist. */
    int jobListLength() const;

    /** @brief Add the job to this collection by pointer. */
    Collection &operator<<(ThreadWeaver::JobInterface *job);

    /** @brief Add the job to this collection. */
    Collection &operator<<(const ThreadWeaver::JobPointer &job);
    Collection &operator<<(JobInterface &job);

protected:
    /** Overload to queue the collection. */
    void aboutToBeQueued_locked(QueueAPI *api) Q_DECL_OVERRIDE;

    /** Overload to dequeue the collection. */
    void aboutToBeDequeued_locked(QueueAPI *api) Q_DECL_OVERRIDE;

    /** Return a ref-erence to the job in the job list at position i. */
    JobPointer jobAt(int i);

    /** Return the number of jobs in the joblist.
     *  Assumes that the mutex is being held.
     */
    virtual int jobListLength_locked() const;

    /** Perform the task usually done when one individual job is
     * finished, but in our case only when the whole collection
     * is finished or partly dequeued.
     */
    void finalCleanup();

protected:
    friend class CollectionExecuteWrapper;
    virtual void elementStarted(JobPointer job, Thread *thread);
    virtual void elementFinished(JobPointer job, Thread *thread);

    //FIXME needed when using BlockExecutionPolicy?
    JobPointer self() const;

protected:
    /** Overload the execute method. */
    void execute(JobPointer job, Thread *) Q_DECL_OVERRIDE;

    /** Overload run().
     * We have to. */
    void run(JobPointer self, Thread *thread) Q_DECL_OVERRIDE;

    /** @brief Enqueue the elements of the collection. */
    virtual void enqueueElements();

private:
    /** Dequeue all elements of the collection.
     * Note: This will not dequeue the collection itself.
     */
    void dequeueElements(bool queueApiIsLocked);

    class Private;
    Private *const d;
};

}

#endif
