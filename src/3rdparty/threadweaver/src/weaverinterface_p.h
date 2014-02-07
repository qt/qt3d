/* -*- C++ -*-

   This file declares the WeaverInterface class.

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

#ifndef THREADWEAVER_QUEUEINTERFACE_H
#define THREADWEAVER_QUEUEINTERFACE_H

#include "jobpointer.h"

namespace ThreadWeaver
{

class Job;
class Thread;

class THREADWEAVER_EXPORT WeaverInterface
{
public:
    virtual ~WeaverInterface() {}

    /** @brief Assign a job to an idle thread.
     * @param th the thread to give a new Job to
     * @param wasBusy true if a job was previously assigned to the calling thread
     */
    virtual JobPointer applyForWork(Thread *th, bool wasBusy) = 0;

    /** @brief Wait (by suspending the calling thread) until a job becomes available. */
    virtual void waitForAvailableJob(Thread *th) = 0;
};

}

#endif // THREADWEAVER_QUEUEINTERFACE_H
