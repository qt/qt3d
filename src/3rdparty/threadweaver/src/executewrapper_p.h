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

#ifndef EXECUTEWRAPPER_H
#define EXECUTEWRAPPER_H

#include <QAtomicPointer>

#include "job.h"
#include "executor_p.h"

namespace ThreadWeaver
{

class Job;
class Thread;

//FIXME Pimpl, make part of the API, document
//Find a way to avoid new/delete operation, this is rather performance sensitive area
class ExecuteWrapper : public Executor
{
public:
    ExecuteWrapper();
    Executor *wrap(Executor *previous);
    Executor *unwrap(JobPointer job);
    void begin(JobPointer job, Thread *) Q_DECL_OVERRIDE;
    void execute(JobPointer job, Thread *thread) Q_DECL_OVERRIDE;
    void executeWrapped(JobPointer job, Thread *thread);
    void end(JobPointer job, Thread *) Q_DECL_OVERRIDE;

private:
    QAtomicPointer<Executor> wrapped;
};

}

#endif // EXECUTEWRAPPER_H
