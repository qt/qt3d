/* -*- C++ -*-

   Class to manipulate job execution in ThreadWeaver.

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

#include "executewrapper_p.h"

namespace ThreadWeaver
{

ExecuteWrapper::ExecuteWrapper()
{
}

Executor *ExecuteWrapper::wrap(Executor *previous)
{
    return wrapped.fetchAndStoreOrdered(previous);
}

Executor *ExecuteWrapper::unwrap(JobPointer job)
{
    Executor *executor = job->setExecutor(wrapped.fetchAndAddOrdered(0));
    Q_ASSERT_X(executor == this, Q_FUNC_INFO, "ExecuteWrapper can only unwrap itself!");
    wrapped.fetchAndStoreOrdered(0);
    return executor;
}

void ExecuteWrapper::begin(JobPointer job, Thread *thread)
{
    Q_ASSERT(wrapped.loadAcquire() != 0);
    wrapped.loadAcquire()->begin(job, thread);
}

void ExecuteWrapper::execute(JobPointer job, Thread *thread)
{
    executeWrapped(job, thread);
}

void ExecuteWrapper::executeWrapped(JobPointer job, Thread *thread)
{
    Executor *executor = wrapped.loadAcquire();
    Q_ASSERT_X(executor != 0, Q_FUNC_INFO, "Wrapped Executor cannot be zero!");
    executor->execute(job, thread);
}

void ExecuteWrapper::end(JobPointer job, Thread *thread)
{
    Q_ASSERT(wrapped.loadAcquire() != 0);
    wrapped.loadAcquire()->end(job, thread);
}

}

#include "executewrapper_p.h"
