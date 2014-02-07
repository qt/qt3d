/* -*- C++ -*-

   This file implements the InConstructionState class.

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

   $Id: InConstructionState.cpp 30 2005-08-16 16:16:04Z mirko $
*/

#include "inconstructionstate_p.h"

#include "weaver_p.h"
#include "threadweaver.h"

namespace ThreadWeaver
{

InConstructionState::InConstructionState(QueueSignals *weaver)
    : WeaverImplState(weaver)
{
}

void InConstructionState::suspend()
{
    // this request is not handled in InConstruction state
}

void InConstructionState::resume()
{
    // this request is not handled in InConstruction state
}

JobPointer InConstructionState::applyForWork(Thread *th, bool wasBusy)
{
    Q_ASSERT(wasBusy == false);
    // As long as we are in the construction state, no jobs will be given
    // to the worker threads. The threads will be suspended. They will
    // return from the blocked state when jobs are queued. By then, we
    // should not be in InConstruction state anymore, and we hand the job
    // application over to the then active state.
    while (weaver()->state()->stateId() == InConstruction) {
        weaver()->waitForAvailableJob(th);
    }
    return weaver()->applyForWork(th, wasBusy);
}

StateId InConstructionState::stateId() const
{
    return InConstruction;
}

}
