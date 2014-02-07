/* -*- C++ -*-

This file implements the DestructedState class.

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

$Id: DestructedState.cpp 30 2005-08-16 16:16:04Z mirko $
*/

#include "destructedstate_p.h"

namespace ThreadWeaver
{

DestructedState::DestructedState(QueueSignals *)
    : WeaverImplState(0) // make sure we cannot use weaver, ever :-)
{
}

void DestructedState::shutDown()
{
}

Weaver *DestructedState::weaver()
{
    return 0;
}

const Weaver *DestructedState::weaver() const
{
    return 0;
}

void DestructedState::setMaximumNumberOfThreads(int)
{
}

int DestructedState::maximumNumberOfThreads() const
{
    return 0;
}

int DestructedState::currentNumberOfThreads() const
{
    return 0;
}

void DestructedState::enqueue(const QVector<JobPointer> &)
{
}

bool DestructedState::dequeue(const JobPointer &)
{
    return false;
}

void DestructedState::dequeue()
{
}

void DestructedState::finish()
{
}

bool DestructedState::isEmpty() const
{
    return true;
}

bool DestructedState::isIdle() const
{
    return true;
}

int DestructedState::queueLength() const
{
    return 0;
}

void DestructedState::requestAbort()
{
}

void DestructedState::suspend()
{
}

void DestructedState::resume()
{
}

JobPointer DestructedState::applyForWork(Thread *, bool wasBusy)
{
    Q_UNUSED(wasBusy) // except in Q_ASSERT
    Q_ASSERT(wasBusy == false);
    return JobPointer();
}

void DestructedState::waitForAvailableJob(Thread *)
{
}

StateId DestructedState::stateId() const
{
    return Destructed;
}

}
