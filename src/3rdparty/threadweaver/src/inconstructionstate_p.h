/* -*- C++ -*-

   This file declares the InConstructionState class.

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

   $Id: InConstructionState.h 32 2005-08-17 08:38:01Z mirko $
*/

#ifndef InConstructionState_H
#define InConstructionState_H

#include "weaverimplstate_p.h"

namespace ThreadWeaver
{

class Weaver;
class Queue;

/** InConstructionState handles the calls to the Weaver
        object until the constructor has finished.
    */
class InConstructionState : public WeaverImplState
{
public:
    explicit InConstructionState(QueueSignals *weaver);
    /** Suspend job processing. */
    void suspend() Q_DECL_OVERRIDE;
    /** Resume job processing. */
    void resume() Q_DECL_OVERRIDE;
    /** Assign a job to an idle thread. */
    JobPointer applyForWork(Thread *th, bool wasBusy) Q_DECL_OVERRIDE;
    /** reimpl */
    StateId stateId() const Q_DECL_OVERRIDE;
};

}

#endif // InConstructionState_H
