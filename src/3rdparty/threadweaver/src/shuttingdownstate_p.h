/* -*- C++ -*-

   This file declares the ShuttingDownState class.

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

   $Id: ShuttingDownState.h 32 2005-08-17 08:38:01Z mirko $
*/

#ifndef ShuttingDownState_H
#define ShuttingDownState_H

#include "weaverimplstate_p.h"

namespace ThreadWeaver
{

class Queue;

/** ShuttingDownState is enabled when the Weaver destructor is entered. It
 *  prevents threads from still accessing queue management methods, and new jobs being queued.
 */
class ShuttingDownState : public WeaverImplState
{
public:
    explicit ShuttingDownState(QueueSignals *weaver);

    /** Shut down the queue. */
    void shutDown() Q_DECL_OVERRIDE;
    /** Suspend job processing. */
    void suspend() Q_DECL_OVERRIDE;
    /** Resume job processing. */
    void resume() Q_DECL_OVERRIDE;
    /** Assign a job to an idle thread. */
    JobPointer applyForWork(Thread *th, bool wasBusy) Q_DECL_OVERRIDE;
    /** Wait (by suspending the calling thread) until a job becomes available. */
    void waitForAvailableJob(Thread *th) Q_DECL_OVERRIDE;
    /** reimpl */
    StateId stateId() const Q_DECL_OVERRIDE;
};

}

#endif // ShuttingDownState_H
