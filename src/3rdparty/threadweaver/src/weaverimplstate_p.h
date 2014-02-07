/* -*- C++ -*-

   This file declares the StateIMplementation class.

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

   $Id$
*/

#ifndef STATEIMPLEMENTATION_H
#define STATEIMPLEMENTATION_H

#include "state.h"
#include "weaver_p.h"

namespace ThreadWeaver
{

class QueueSignals;

/** @brief Base class for all WeaverImpl states. */
class WeaverImplState : public State
{
public:
    explicit WeaverImplState(QueueSignals *weaver);

    const State *state() const Q_DECL_OVERRIDE;

    /** Shut down the queue. */
    void shutDown() Q_DECL_OVERRIDE;
    /** Set the maximum number of threads this Weaver object may start. */
    void setMaximumNumberOfThreads(int cap) Q_DECL_OVERRIDE;
    /** Get the maximum number of threads this Weaver may start. */
    int maximumNumberOfThreads() const Q_DECL_OVERRIDE;
    /** Returns the current number of threads in the inventory. */
    int currentNumberOfThreads() const Q_DECL_OVERRIDE;
    /** Enqueue a job. */
    void enqueue(const QVector<JobPointer> &jobs) Q_DECL_OVERRIDE;
    /** Dequeue a job. */
    bool dequeue(const JobPointer &job) Q_DECL_OVERRIDE;
    /** Dequeue all jobs. */
    void dequeue() Q_DECL_OVERRIDE;
    /** Finish all queued jobs. */
    void finish() Q_DECL_OVERRIDE;
    /** Are no more jobs queued? */
    bool isEmpty() const Q_DECL_OVERRIDE;
    /** Are all threads waiting? */
    bool isIdle() const Q_DECL_OVERRIDE;
    /** How many jobs are currently queued? */
    int queueLength() const Q_DECL_OVERRIDE;
    /** Request abort for all queued and currently executed jobs. */
    void requestAbort() Q_DECL_OVERRIDE;
    /** Reschedule jobs to threads. */
    void reschedule() Q_DECL_OVERRIDE;
    /** Wait (by suspending the calling thread) until a job becomes available. */
    void waitForAvailableJob(Thread *th) Q_DECL_OVERRIDE;

protected:
    /** Provide correct return type for WeaverImpl states. */
    Weaver *weaver() Q_DECL_OVERRIDE;
    const Weaver *weaver() const Q_DECL_OVERRIDE;
};

}

#endif
