/* -*- C++ -*-

   This file implements the ShuttingDownState class.

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

   $Id: ShuttingDownState.cpp 30 2005-08-16 16:16:04Z mirko $
*/

#include "shuttingdownstate_p.h"

namespace ThreadWeaver
{

ShuttingDownState::ShuttingDownState(QueueSignals *weaver)
    : WeaverImplState(weaver)
{
}

void ShuttingDownState::shutDown()
{
}

void ShuttingDownState::suspend()
{
    // ignored: when shutting down, we do not return to the suspended state
}

void ShuttingDownState::resume()
{
    // ignored: when shutting down, we do not return from the suspended state
}

JobPointer ShuttingDownState::applyForWork(Thread *, bool wasBusy)
{
    Q_UNUSED(wasBusy) // except in Q_ASSERT
    Q_ASSERT(wasBusy == false);
    return JobPointer();  // tell threads to exit
}

void ShuttingDownState::waitForAvailableJob(Thread *)
{
    // immidiately return here
}

StateId ShuttingDownState::stateId() const
{
    return ShuttingDown;
}

}
