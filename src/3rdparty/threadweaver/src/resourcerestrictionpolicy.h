/* -*- C++ -*-

This file declares the ResourceRestrictionPolicy class.

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

#ifndef RESOURCE_RESTRICTION_POLICY_H
#define RESOURCE_RESTRICTION_POLICY_H

#include <QtGlobal>

#include "jobpointer.h"
#include "queuepolicy.h"

namespace ThreadWeaver
{

class JobInterface;

/** @brief ResourceRestrictionPolicy is used to limit the number of concurrent accesses to the same resource.
 *
 *  If a set of Jobs accesses a resource that can be overloaded, this may degrade application performance. For
 *  example, loading too many files from the hard disc at the same time may lead to longer load times.
 *  ResourceRestrictionPolicy can be used to cap the number of accesses. Resource restriction policies are
 *  shared between the affected jobs. All jobs that share a resurce restriction policy have to acquire
 *  permission from the policy before they can run. In this way, resource restrictions can be compared to
 *  semaphores, only that they require no locking at the thread level.
 *  The SMIV example uses a resource restriction to limit the number of images files that are loaded from
 *  the disk at the same time.
 */

class THREADWEAVER_EXPORT ResourceRestrictionPolicy : public QueuePolicy
{
public:
    explicit ResourceRestrictionPolicy(int cap = 0);
    ~ResourceRestrictionPolicy();

    /** @brief Cap the number of simultaniously executing jobs.
     *  Capping the amount of jobs will make sure that at max the number of jobs executing at any time is
     *  limited to the capped amount. Note that immediately after setting the amount of running jobs may be
     *  higher than the set amount. This setting only limits the starting of new jobs.
     *  @param newCap the new cap to limit the amount of parallel jobs.
     */
    void setCap(int newCap);
    int cap() const;
    bool canRun(JobPointer) Q_DECL_OVERRIDE;
    void free(JobPointer) Q_DECL_OVERRIDE;
    void release(JobPointer) Q_DECL_OVERRIDE;
    void destructed(JobInterface *job) Q_DECL_OVERRIDE;

private:
    class Private;
    Private *const d;
};

}

#endif // RESOURCE_RESTRICTION_POLICY_H
