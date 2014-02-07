/* -*- C++ -*-

This file implements the SuspendedState class.

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

$Id: SuspendedState.cpp 30 2005-08-16 16:16:04Z mirko $
*/

#include "suspendedstate_p.h"

#include "threadweaver.h"
#include "weaver_p.h"

namespace ThreadWeaver
{

SuspendedState::SuspendedState(Weaver *weaver)
    : WeaverImplState(weaver)
{
}

void SuspendedState::suspend()
{
    // this request is not handled in Suspended state
}

void SuspendedState::resume()
{
    weaver()->setState(WorkingHard);
}

JobPointer SuspendedState::applyForWork(Thread *th, bool wasBusy)
{
    // suspend all threads in case they wake up:
    Q_ASSERT(wasBusy == 0);
    weaver()->waitForAvailableJob(th);
    return weaver()->applyForWork(th, wasBusy);
}

StateId SuspendedState::stateId() const
{
    return Suspended;
}

}
