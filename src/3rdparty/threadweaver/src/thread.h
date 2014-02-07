/* -*- C++ -*-

   This file declares the Thread class.

   Thread is not a part of the public interface of the ThreadWeaver library.

   $ Author: Mirko Boehm $
   $ Copyright: (C) 2004-2013 Mirko Boehm $
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

   $Id: Thread.h 32 2005-08-17 08:38:01Z mirko $
*/

#ifndef THREADWEAVER_THREAD_H
#define THREADWEAVER_THREAD_H

#include <QtCore/QMutex>
#include <QtCore/QThread>

#include "threadweaver_export.h"
#include "jobpointer.h"

namespace ThreadWeaver
{

class Job;
class Weaver;

/** The class Thread is used to represent the worker threads in
    the weaver's inventory. It is not meant to be overloaded. */
class THREADWEAVER_EXPORT Thread : public QThread
{
    Q_OBJECT

public:
    /** Create a thread.
        These thread objects are only used inside the Weaver parent
        object.

            @param parent the parent Weaver
        */
    explicit Thread(Weaver *parent = 0);

    /** The destructor. */
    ~Thread();

    /** Overload to execute the assigned jobs.
    Whenever the thread is idle, it will ask its Weaver parent for a
    Job to do. The Weaver will either return a Job or a Nil
    pointer. When a Nil pointer is returned, it tells the thread to
    exit.
    */
    void run() Q_DECL_OVERRIDE;

    /** Returns the thread id.
        This id marks the respective Thread object, and must
        therefore not be confused with, e.g., the pthread thread
        ID.

        Generally, the way threads are implemented is not
        specified. id() is the only way to uniquely identify a
        thread within ThreadWeaver.
        */
    unsigned int id();

    /** Request the abortion of the current job.
        If there is no current job, this method will do nothing, but can
        safely be called.
        It forwards the request to the current Job.
     */
    void requestAbort();

Q_SIGNALS:
    /** The thread has been started. */
    void started(ThreadWeaver::Thread *);
    /** The thread started to process a job. */
    void jobStarted(ThreadWeaver::JobPointer, ThreadWeaver::Thread *);
    /** The thread finished to execute a job. */
    void jobDone(ThreadWeaver::JobPointer);

private:
    class Private;
    Private *const d;
};

}

#endif

