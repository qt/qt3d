/* -*- C++ -*-

   This file implements the ResourceRestrictionPolicy class.

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

   $Id: Job.h 32 2005-08-17 08:38:01Z mirko $
*/

#include "resourcerestrictionpolicy.h"

#include <QtCore/QList>
#include <QtCore/QMutex>

#include "debuggingaids.h"
#include "managedjobpointer.h"

using namespace ThreadWeaver;

class ResourceRestrictionPolicy::Private
{
public:
    Private(int theCap)
        : cap(theCap)
    {}
    QMutex *mutex()
    {
        return &mutex_;
    }

    int cap;
    QList<JobPointer> customers;
    QMutex mutex_;
};

ResourceRestrictionPolicy::ResourceRestrictionPolicy(int cap)
    : QueuePolicy()
    , d(new Private(cap))
{
}

ResourceRestrictionPolicy::~ResourceRestrictionPolicy()
{
    delete d;
}

void ResourceRestrictionPolicy::setCap(int cap)
{
    QMutexLocker l(d->mutex());
    d->cap = cap;
}

int ResourceRestrictionPolicy::cap() const
{
    QMutexLocker l(d->mutex());
    return d->cap;
}

bool ResourceRestrictionPolicy::canRun(JobPointer job)
{
    QMutexLocker l(d->mutex());
    if (d->customers.size() < d->cap) {
        d->customers.append(job);
        return true;
    } else {
        return false;
    }
}

void ResourceRestrictionPolicy::free(JobPointer job)
{
    QMutexLocker l(d->mutex());
    int position = d->customers.indexOf(job);

    if (position != -1) {
        debug(4, "ResourceRestrictionPolicy::free: job %p done.\n", (void *)job.data());
        d->customers.removeAt(position);
    }
}

void ResourceRestrictionPolicy::release(JobPointer job)
{
    free(job);
}

void ResourceRestrictionPolicy::destructed(JobInterface *job)
{
    free(ManagedJobPointer<JobInterface>(job));
}
