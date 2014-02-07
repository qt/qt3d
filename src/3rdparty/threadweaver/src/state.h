/* -*- C++ -*-

   This file implements the state handling in ThreadWeaver.

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

   $Id: State.h 32 2005-08-17 08:38:01Z mirko $
*/

#ifndef THREADWEAVER_STATE_H
#define THREADWEAVER_STATE_H

#include <QString>

#include "queueinterface.h"
#include "weaverinterface_p.h"
#include "queuesignals.h"
#include "threadweaver_export.h"

namespace ThreadWeaver
{

class Job;
class Thread;

/** All weaver objects maintain a state of operation which can be
 *  queried by the application. See the threadweaver documentation on
 *  how the different states are related.
 */

enum StateId {
    /** The object is in the state of construction and has not yet
     *  started to process jobs. */
    InConstruction = 0,
    /** Jobs are being processed. */
    WorkingHard,
    /** Job processing is suspended, but some jobs which where already
     *  in progress are not finished yet. */
    Suspending,
    /** Job processing is suspended, and no jobs are being
     *  processed. */
    Suspended,
    /** The object is being destructed. Jobs might still be processed,
     *  the destructor will wait for all threads to exit and then
     *  end. */
    ShuttingDown,
    /** The object is being destructed, and all threads have
     *  exited. No jobs are handled anymore. */
    Destructed,
    /** Not a state, but a sentinel for the number of defined states. */
    NoOfStates
};

/** We use a State pattern to handle the system state in ThreadWeaver. */
class THREADWEAVER_EXPORT State : public QueueInterface, public WeaverInterface
{
public:
    /** Default constructor. */
    explicit State(QueueSignals *weaver);

    /** Destructor. */
    virtual ~State();

    /** The ID of the current state.
     *  @see StateNames, StateID
     */
    QString stateName() const;

    /** The state Id. */
    virtual StateId stateId() const = 0;

    /** The state has been changed so that this object is responsible for
     *  state handling. */
    virtual void activated();

protected:
    /** The Weaver interface this state handles. */
    virtual QueueInterface *weaver();
    virtual const QueueInterface *weaver() const;

private:
    class Private;
    Private *const d;
};

}

#endif // THREADWEAVER_STATE_H
