/* -*- C++ -*-

   Class to manipulate job execution in ThreadWeaver.

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

#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "job.h"

namespace ThreadWeaver
{

class Job;
class Thread;

//FIXME Pimpl, make part of the API, document
//TODO can JobPointer references be used?
class Executor
{
public:
    virtual ~Executor();
    virtual void begin(JobPointer, Thread *) = 0;
    void defaultBegin(JobPointer job, Thread *thread);
    virtual void execute(JobPointer, Thread *) = 0;
    virtual void end(JobPointer, Thread *) = 0;
    void defaultEnd(JobPointer job, Thread *thread);
    virtual void cleanup(JobPointer, Thread *);

    void run(JobPointer job, Thread *thread);
};

}

#endif // EXECUTOR_H
