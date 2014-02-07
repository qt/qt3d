/* -*- C++ -*-

   This file declares the DestructedState class.

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

   $Id: DestructedState.h 32 2005-08-17 08:38:01Z mirko $
*/

#ifndef DestructedState_H
#define DestructedState_H

#include "weaverimplstate_p.h"

namespace ThreadWeaver
{

class QueueSignals;

/** DestructedState is only active after the thread have been destroyed by
 *  the destructor, but before superclass destructors have finished.
 */
class DestructedState : public WeaverImplState
{
public:
    explicit DestructedState(QueueSignals *weaver);
    void shutDown() Q_DECL_OVERRIDE;
    Weaver *weaver() Q_DECL_OVERRIDE;
    const Weaver *weaver() const Q_DECL_OVERRIDE;
    void setMaximumNumberOfThreads(int cap) Q_DECL_OVERRIDE;
    int maximumNumberOfThreads() const Q_DECL_OVERRIDE;
    int currentNumberOfThreads() const Q_DECL_OVERRIDE;
    void enqueue(const QVector<JobPointer> &job) Q_DECL_OVERRIDE;
    bool dequeue(const JobPointer &job) Q_DECL_OVERRIDE;
    void dequeue() Q_DECL_OVERRIDE;
    void finish() Q_DECL_OVERRIDE;
    bool isEmpty() const Q_DECL_OVERRIDE;
    bool isIdle() const Q_DECL_OVERRIDE;
    int queueLength() const Q_DECL_OVERRIDE;
    void requestAbort() Q_DECL_OVERRIDE;
    void suspend() Q_DECL_OVERRIDE;
    void resume() Q_DECL_OVERRIDE;
    JobPointer applyForWork(Thread *th, bool wasBusy) Q_DECL_OVERRIDE;
    void waitForAvailableJob(Thread *th) Q_DECL_OVERRIDE;
    StateId stateId() const Q_DECL_OVERRIDE;
};

}

#endif // DestructedState_H
