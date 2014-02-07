/* -*- C++ -*-

   This file implements the SuspendingState class.

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

   $Id: SuspendingState.cpp 30 2005-08-16 16:16:04Z mirko $
*/

#include "suspendingstate_p.h"

#include "state.h"
#include "weaver_p.h"
#include "threadweaver.h"

namespace ThreadWeaver
{

SuspendingState::SuspendingState(Weaver *weaver)
    : WeaverImplState(weaver)
{
}

void SuspendingState::suspend()
{
    // this request is not handled in Suspending state (we are already suspending...)
}

void SuspendingState::resume()
{
    weaver()->setState(WorkingHard);
}

void SuspendingState::activated()
{
    weaver()->reschedule();
}

JobPointer SuspendingState::applyForWork(Thread *th, bool wasBusy)
{
    weaver()->takeFirstAvailableJobOrSuspendOrWait(th, wasBusy, true, true);
    weaver()->waitForAvailableJob(th);
    return weaver()->applyForWork(th, false);
}

StateId SuspendingState::stateId() const
{
    return Suspending;
}

}
