/* -*- C++ -*-

   The detailed API for Weavers in ThreadWeaver.

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

*/

#ifndef QUEUEAPI_H
#define QUEUEAPI_H

#include "queuesignals.h"
#include "weaverinterface_p.h"
#include "state.h"
#include "jobpointer.h"

namespace ThreadWeaver
{

class QueueAPI : public QueueSignals, public WeaverInterface
{
    Q_OBJECT

public:
    explicit QueueAPI(QObject *parent = 0);

    virtual void shutDown_p() = 0;
    virtual const State *state() const = 0;
    virtual State *state() = 0;
    virtual void setMaximumNumberOfThreads_p(int cap) = 0;
    virtual int maximumNumberOfThreads_p() const = 0;
    virtual int currentNumberOfThreads_p() const = 0;
    virtual bool dequeue_p(JobPointer) = 0;
    virtual void dequeue_p() = 0;
    virtual void finish_p() = 0;
    virtual void suspend_p() = 0;
    virtual void resume_p() = 0;
    virtual bool isEmpty_p() const = 0;
    virtual bool isIdle_p() const = 0;
    virtual int queueLength_p() const = 0;
    virtual void requestAbort_p() = 0;
};

}

#endif // QUEUEAPI_H
