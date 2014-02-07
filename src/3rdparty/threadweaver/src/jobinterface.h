/* -*- C++ -*-

   Interface for jobs in ThreadWeaver.

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

#ifndef JOBINTERFACE_H
#define JOBINTERFACE_H

#include <QSharedPointer>

#include "threadweaver_export.h"
#include "jobpointer.h"

class QMutex;

namespace ThreadWeaver
{

class Thread;
class Executor;
class QueueAPI;
class QueuePolicy;

class JobInterface;
typedef QSharedPointer<JobInterface> JobPointer;

class THREADWEAVER_EXPORT JobInterface
{
public:
    enum Status {
        Status_NoStatus = 0,
        Status_New,
        Status_Queued,
        Status_Running,
        Status_Success,
        Status_Failed,
        Status_Aborted,
        Status_NumberOfStatuses,
    };

    virtual ~JobInterface() {}
    virtual void execute(JobPointer job, Thread *) = 0;
    virtual void blockingExecute() = 0;
    virtual Executor *setExecutor(Executor *executor) = 0;
    virtual Executor *executor() const = 0;
    virtual int priority() const = 0;
    virtual Status status() const = 0;
    virtual void setStatus(Status) = 0;
    virtual bool success() const = 0;
    virtual void requestAbort() = 0;
    virtual void aboutToBeQueued(QueueAPI *api) = 0;
    virtual void aboutToBeQueued_locked(QueueAPI *api) = 0;
    virtual void aboutToBeDequeued(QueueAPI *api) = 0;
    virtual void aboutToBeDequeued_locked(QueueAPI *api) = 0;
    virtual bool isFinished() const = 0;
    virtual void assignQueuePolicy(QueuePolicy *) = 0;
    virtual void removeQueuePolicy(QueuePolicy *) = 0;
    virtual QList<QueuePolicy *> queuePolicies() const = 0;
    virtual void freeQueuePolicyResources(JobPointer) = 0;
    virtual void run(JobPointer self, Thread *thread) = 0;
    friend class Executor;
    virtual void defaultBegin(JobPointer job, Thread *thread) = 0;
    virtual void defaultEnd(JobPointer job, Thread *thread) = 0;
    virtual QMutex *mutex() const = 0;
};

}

Q_DECLARE_METATYPE(ThreadWeaver::JobPointer)

#endif // JOBINTERFACE_H
