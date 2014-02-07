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

#include "collection.h"
#include "sequence.h"
#include "managedjobpointer.h"

#include "iddecorator.h"

namespace ThreadWeaver
{

const int IdDecorator_AutoDelete = 1;

// Pssst: IdDecorator uses the d pointer to hold decoratee. It also uses d2 as a bitfield to store the
// autoDelete setting. The goal is not to require a dynamic allocation on creation.
IdDecorator::IdDecorator(JobInterface *decoratee, bool autoDelete)
    : d1(reinterpret_cast<Private1 *>(decoratee))
    , d2(0)
{
    setAutoDelete(autoDelete);
}

IdDecorator::~IdDecorator()
{
    // Do not assert here. IdDecorator can decorate a null pointer. Only assert if a method is called on a decorared
    // null  pointer.
    if (autoDelete()) {
        delete job();
    }
}

QMutex *IdDecorator::mutex() const
{
    Q_ASSERT(d1);
    return job()->mutex();
}

void IdDecorator::run(JobPointer self, Thread *thread)
{
    Q_ASSERT(d1);
    job()->run(self, thread);
}

void IdDecorator::defaultBegin(JobPointer self, Thread *thread)
{
    Q_ASSERT(d1);
    job()->defaultBegin(self, thread);
}

void IdDecorator::defaultEnd(JobPointer self, Thread *thread)
{
    Q_ASSERT(d1);
    job()->defaultEnd(self, thread);
}

void IdDecorator::freeQueuePolicyResources(JobPointer j)
{
    Q_ASSERT(d1);
    job()->freeQueuePolicyResources(j);
}

void IdDecorator::removeQueuePolicy(QueuePolicy *policy)
{
    Q_ASSERT(d1);
    job()->removeQueuePolicy(policy);
}

QList<QueuePolicy *> IdDecorator::queuePolicies() const
{
    Q_ASSERT(d1);
    return job()->queuePolicies();
}

void IdDecorator::assignQueuePolicy(QueuePolicy *policy)
{
    Q_ASSERT(d1);
    job()->assignQueuePolicy(policy);
}

bool IdDecorator::isFinished() const
{
    Q_ASSERT(d1);
    return job()->isFinished();
}

void IdDecorator::aboutToBeQueued(QueueAPI *api)
{
    Q_ASSERT(d1);
    job()->aboutToBeQueued(api);
}

void IdDecorator::aboutToBeQueued_locked(QueueAPI *api)
{
    Q_ASSERT(d1);
    job()->aboutToBeQueued_locked(api);
}

void IdDecorator::aboutToBeDequeued(QueueAPI *api)
{
    Q_ASSERT(d1);
    job()->aboutToBeDequeued(api);
}

void IdDecorator::aboutToBeDequeued_locked(QueueAPI *api)
{
    Q_ASSERT(d1);
    job()->aboutToBeDequeued_locked(api);
}

void IdDecorator::requestAbort()
{
    Q_ASSERT(d1);
    job()->requestAbort();
}

bool IdDecorator::success() const
{
    Q_ASSERT(d1);
    return job()->success();
}

int IdDecorator::priority() const
{
    Q_ASSERT(d1);
    return job()->priority();
}

void IdDecorator::setStatus(JobInterface::Status status)
{
    Q_ASSERT(d1);
    job()->setStatus(status);
}

JobInterface::Status IdDecorator::status() const
{
    Q_ASSERT(d1);
    return job()->status();
}

Executor *IdDecorator::executor() const
{
    Q_ASSERT(d1);
    return job()->executor();
}

Executor *IdDecorator::setExecutor(Executor *executor)
{
    Q_ASSERT(d1);
    return job()->setExecutor(executor);
}

void IdDecorator::execute(ThreadWeaver::JobPointer self, ThreadWeaver::Thread *thread)
{
    Q_ASSERT(d1);
    job()->execute(self, thread);
}

void IdDecorator::blockingExecute()
{
    Q_ASSERT(d1);
    job()->blockingExecute();
}

const ThreadWeaver::JobInterface *IdDecorator::job() const
{
    return reinterpret_cast<JobInterface *>(d1);
}

JobInterface *IdDecorator::job()
{
    return reinterpret_cast<JobInterface *>(d1);
}

void IdDecorator::setAutoDelete(bool onOff)
{

    if (onOff) {
        d2 = reinterpret_cast<IdDecorator::Private2 *>(IdDecorator_AutoDelete);
    } else {
        d2 = 0;
    }
}

bool IdDecorator::autoDelete() const
{
    return d2 == reinterpret_cast<IdDecorator::Private2 *>(IdDecorator_AutoDelete);
}

const ThreadWeaver::Collection *IdDecorator::collection() const
{
    return dynamic_cast<const Collection *>(job());
}

Collection *IdDecorator::collection()
{
    return dynamic_cast<Collection *>(job());
}

const Sequence *IdDecorator::sequence() const
{
    return dynamic_cast<const Sequence *>(job());
}

Sequence *IdDecorator::sequence()
{
    return dynamic_cast<Sequence *>(job());
}

}
