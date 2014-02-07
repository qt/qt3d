/* -*- C++ -*-

   This file is part of ThreadWeaver, a KDE framework.

   $ Author: Mirko Boehm $
   $ Copyright: (C) 2013 Mirko Boehm $
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

#ifndef THREADWEAVER_QUEUEING_H
#define THREADWEAVER_QUEUEING_H

#include "jobpointer.h"
#include "managedjobpointer.h"
#include "jobinterface.h"
#include "collection.h"
#include "lambda.h"
#include "qobjectdecorator.h"
#include "queue.h"

namespace ThreadWeaver
{

// make a job that calls a functor, anything that responds to operator()
template<typename T>
QSharedPointer<Lambda<T> > make_job(T t)
{
    QSharedPointer<Lambda<T> > ret(new Lambda<T>(t));
    return ret;
}

// make a job pointer holding a pointer to a Job(Interface)
template<typename T>
inline QSharedPointer<T> make_job(T *job)
{
    JobInterface *test = static_cast<JobInterface *>(job); Q_UNUSED(test);
    return QSharedPointer<T>(job);
}

// make a job pointer holding anything resembling JobInterface
inline JobPointer make_job_raw(JobInterface *job)
{
    return ManagedJobPointer<JobInterface>(job);
}

// enqueue any functor type to the specified queue:
template<typename T>
JobPointer enqueue(Queue *weaver, T t)
{
    JobPointer ret = make_job(t);
    weaver->enqueue(ret);
    return ret;
}

template<typename T>
QSharedPointer<T>  enqueue(Queue *weaver, T *t)
{
    JobInterface *test = static_cast<JobInterface *>(t); Q_UNUSED(test);
    QSharedPointer<T> ret(make_job(t));
    weaver->enqueue(ret);
    return ret;
}

// specialise for JobPointer:
template<>
inline JobPointer enqueue<JobPointer>(Queue *weaver, JobPointer job)
{
    weaver->enqueue(job);
    return job;
}

// convenience overload: enqueue the functor to the global queue:
template<typename T>
JobPointer enqueue(T t)
{
    return enqueue(Queue::instance(), t);
}

// enqueue a raw pointer with no memory management
inline JobPointer enqueue_raw(Queue *weaver, JobInterface *job)
{
    return enqueue(weaver, make_job_raw(job));
}

// overload to enqueue to the global pool
inline JobPointer enqueue_raw(JobInterface *job)
{
    return enqueue(Queue::instance(), make_job_raw(job));
}

}

#endif // THREADWEAVER_QUEUEING_H
