/* -*- C++ -*-

   Base class for job decorators in ThreadWeaver.

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

#ifndef THREADWEAVER_IDDECORATOR_H
#define THREADWEAVER_IDDECORATOR_H

#include <QObject>

#include "threadweaver_export.h"
#include "jobinterface.h"

namespace ThreadWeaver
{

class Collection;
class Sequence;

/** @brief IdDecorator decorates a job without changing it's behaviour.
 *
 *  It is supposed to be used as the base class for actual decorators that do change the behaviour of jobs. */
class THREADWEAVER_EXPORT IdDecorator : public JobInterface
{
public:
    explicit IdDecorator(JobInterface *job, bool autoDelete = true);
    ~IdDecorator();
    /** Retrieve the decorated job. */
    const JobInterface *job() const;
    /** Retrieve the decorated job. */
    JobInterface *job();
    /** Auto-delete the decoratee or not. */
    void setAutoDelete(bool onOff);
    /** Will the decoratee be auto-deleted? */
    bool autoDelete() const;
    /** Retrieve the decorated job as a Collection.
     *  If the decorated Job is not a Collection, 0 is returned. */
    const Collection *collection() const;
    /** Retrieve the decorated job as a Collection.
     *  If the decorated Job is not a Collection, 0 is returned. */
    Collection *collection();
    /** Retrieve the decorated job as a Sequence.
     *  If the decorated Job is not a Sequence, 0 is returned. */
    const Sequence *sequence() const;
    /** Retrieve the decorated job as a Sequence.
     *  If the decorated Job is not a Sequence, 0 is returned. */
    Sequence *sequence();

    void execute(JobPointer job, Thread *) Q_DECL_OVERRIDE;
    void blockingExecute() Q_DECL_OVERRIDE;
    Executor *setExecutor(Executor *executor) Q_DECL_OVERRIDE;
    Executor *executor() const Q_DECL_OVERRIDE;
    int priority() const Q_DECL_OVERRIDE;
    void setStatus(Status) Q_DECL_OVERRIDE;
    Status status() const Q_DECL_OVERRIDE;
    bool success() const Q_DECL_OVERRIDE;
    void requestAbort() Q_DECL_OVERRIDE;
    void aboutToBeQueued(QueueAPI *api) Q_DECL_OVERRIDE;
    void aboutToBeQueued_locked(QueueAPI *api) Q_DECL_OVERRIDE;
    void aboutToBeDequeued(QueueAPI *api) Q_DECL_OVERRIDE;
    void aboutToBeDequeued_locked(QueueAPI *api) Q_DECL_OVERRIDE;
    bool isFinished() const Q_DECL_OVERRIDE;
    void assignQueuePolicy(QueuePolicy *) Q_DECL_OVERRIDE;
    void removeQueuePolicy(QueuePolicy *) Q_DECL_OVERRIDE;
    QList<QueuePolicy *> queuePolicies() const Q_DECL_OVERRIDE;

protected:
    void freeQueuePolicyResources(JobPointer) Q_DECL_OVERRIDE;
    void run(JobPointer self, Thread *thread) Q_DECL_OVERRIDE;
    void defaultBegin(JobPointer job, Thread *thread) Q_DECL_OVERRIDE;
    void defaultEnd(JobPointer job, Thread *thread) Q_DECL_OVERRIDE;

    QMutex *mutex() const Q_DECL_OVERRIDE;

private:
    class Private1;
    Private1 *const d1;
    class Private2;
    Private2 *d2;
};

}

#endif // THREADWEAVER_IDDECORATOR_H
