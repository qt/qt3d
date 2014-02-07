/* -*- C++ -*-

   This file declares the WorkingHardState class.

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

   $Id: WorkingHardState.h 32 2005-08-17 08:38:01Z mirko $
*/

#ifndef WorkingHardState_H
#define WorkingHardState_H

#include "weaverimplstate_p.h"
#include "weaver_p.h"

namespace ThreadWeaver
{

/* WorkingHardState handles the state dependent calls in the state where
 * jobs are executed.  */
class WorkingHardState : public WeaverImplState
{
public:
    explicit WorkingHardState(Weaver *weaver);
    /** Suspend job processing. */
    void suspend() Q_DECL_OVERRIDE;
    /** Resume job processing. */
    void resume() Q_DECL_OVERRIDE;
    /** Assign a job to an idle thread. */
    JobPointer applyForWork(Thread *th, bool wasBusy) Q_DECL_OVERRIDE;
    /** Overload. */
    void activated() Q_DECL_OVERRIDE;
    /** reimpl */
    StateId stateId() const Q_DECL_OVERRIDE;
};

}

#endif // WorkingHardIState_H
