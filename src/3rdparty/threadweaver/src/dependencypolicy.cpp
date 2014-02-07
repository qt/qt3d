/* -*- C++ -*-

This file implements the DependencyPolicy class.

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

$Id: DebuggingAids.cpp 20 2005-08-08 21:02:51Z mirko $
*/

#include "dependencypolicy.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QMutex>
#include <QtCore/QDebug>

#include "job.h"
#include "debuggingaids.h"
#include "managedjobpointer.h"

#include "dependency.h"

using namespace ThreadWeaver;

typedef QMultiMap<JobPointer, JobPointer> JobMultiMap;

class DependencyPolicy::Private
{
public:
    /** A container to keep track of Job dependencies.
     *  For each dependency A->B, which means Job B depends on Job A and may only be executed after A has been
     *  finished, an entry will be added with key A and value B. When A is finished, the entry will be removed.
     */
    JobMultiMap &dependencies()
    {
        return depMap_;
    }

    QMutex *mutex()
    {
        return &mutex_;
    }

    JobMultiMap depMap_;
    QMutex mutex_;
};

DependencyPolicy::DependencyPolicy()
    : QueuePolicy()
    , d(new Private())
{
}

DependencyPolicy::~DependencyPolicy()
{
    delete d;
}

void DependencyPolicy::addDependency(JobPointer jobA, JobPointer jobB)
{
    // jobA depends on jobB
    REQUIRE(jobA != 0 && jobB != 0 && jobA != jobB);

    jobA->assignQueuePolicy(this);
    jobB->assignQueuePolicy(this);
    QMutexLocker l(d->mutex());
    d->dependencies().insert(jobA, jobB);

    ENSURE(d->dependencies().contains(jobA));
}

void DependencyPolicy::addDependency(const Dependency &dep)
{
    addDependency(dep.dependent(), dep.dependee());
}

bool DependencyPolicy::removeDependency(JobPointer jobA, JobPointer jobB)
{
    REQUIRE(jobA != 0 && jobB != 0);
    bool result = false;
    QMutexLocker l(d->mutex());

    // there may be only one (!) occurrence of [this, dep]:
    QMutableMapIterator<JobPointer, JobPointer> it(d->dependencies());
    while (it.hasNext()) {
        it.next();
        if (it.key() == jobA && it.value() == jobB) {
            it.remove();
            result = true;
            break;
        }
    }

    ENSURE(! d->dependencies().keys(jobB).contains(jobA));
    return result;
}

bool DependencyPolicy::removeDependency(const Dependency &dep)
{
    return removeDependency(dep.dependent(), dep.dependee());
}

void DependencyPolicy::resolveDependencies(JobPointer job)
{
    if (job->success()) {
        QMutexLocker l(d->mutex());
        QMutableMapIterator<JobPointer, JobPointer> it(d->dependencies());
        // there has to be a better way to do this: (?)
        while (it.hasNext()) { // we remove all entries where jobs depend on *this* :
            it.next();
            if (it.value() == job) {
                it.remove();
            }
        }
    }
}

//QList<JobPointer> DependencyPolicy::getDependencies(JobPointer job) const
//{
//    REQUIRE (job != 0);
//    QList<JobInterface*> result;
//    JobMultiMap::const_iterator it;
//    QMutexLocker l( & d->mutex() );

//    for ( it = d->dependencies().constBegin(); it != d->dependencies().constEnd(); ++it )
//    {
//        if ( it.key() == job )
//        {
//            result.append( it.value() );
//        }
//    }
//    return result;
//}

bool DependencyPolicy::hasUnresolvedDependencies(JobPointer job) const
{
    REQUIRE(job != 0);
    QMutexLocker l(d->mutex());
    return d->dependencies().contains(job);
}

DependencyPolicy &DependencyPolicy::instance()
{
    static DependencyPolicy policy;
    return policy;
}

bool DependencyPolicy::canRun(JobPointer job)
{
    REQUIRE(job != 0);
    return !hasUnresolvedDependencies(job);
}

void DependencyPolicy::free(JobPointer job)
{
    REQUIRE(job != 0);
    REQUIRE(job->status() > Job::Status_Running);
    if (job->success()) {
        resolveDependencies(job);
        debug(3, "DependencyPolicy::free: dependencies resolved for job %p.\n", (void *)job.data());
    } else {
        debug(3, "DependencyPolicy::free: not resolving dependencies for %p (execution not successful).\n",
              (void *)job.data());
    }
    ENSURE((!hasUnresolvedDependencies(job) && job->success()) || !job->success());
}

void DependencyPolicy::release(JobPointer job)
{
    REQUIRE(job != 0); Q_UNUSED(job)
}

void DependencyPolicy::destructed(JobInterface *job)
{
    REQUIRE(job != 0);
    resolveDependencies(ManagedJobPointer<JobInterface>(job));
}

//void DependencyPolicy::dumpJobDependencies()
//{
//    QMutexLocker l( & d->mutex() );

//    debug ( 0, "Job Dependencies (left depends on right side):\n" );
//    for ( JobMultiMap::const_iterator it = d->dependencies().constBegin(); it != d->dependencies().constEnd(); ++it )
//    {
//        debug( 0, "  : %p <-- %p\n", (void*)it.key(), (void*)it.value());
//    }
//    debug ( 0, "-----------------\n" );
//}

