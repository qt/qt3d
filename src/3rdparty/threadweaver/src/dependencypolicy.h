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

#ifndef DEPENDENCYPOLICY_H
#define DEPENDENCYPOLICY_H

#include <QtGlobal>

//template <typename T> class QList;

#include "queuepolicy.h"

namespace ThreadWeaver
{

class JobInterface;
class Dependency;

/** @brief DependencyPolicy implements execution-time dependencies dependencies between Jobs.
 *
 *  To declare that Job B can only be executed when Job A is finished, call addDependency.
 *
 *  Be aware of circular dependencies. All dependencies on a Job will be removed if the Job object is destructed.
 *  Sequence uses dependencies to implement the ordered execution of the sequence elements.
 */
class THREADWEAVER_EXPORT DependencyPolicy : public QueuePolicy
{
public:
    /** Destructor. */
    ~DependencyPolicy();

    /** @brief Add jobB as a dependency of jobA.
     *  jobA will only be executed after jobB has been successfully processed.
     *  @param jobA the depending job
     *  @param jobB the job jobA depends on
     */
    void addDependency(JobPointer jobA, JobPointer jobB);
    void addDependency(const Dependency &dep);

    /** @brief Remove a dependency.
     *  The dependency of jobA on jobB is removed. If no dependencies are left for jobA, canRun will return true.
     *  Returns false if the given object is not dependency of this job.
     *  @param jobA the depending job
     *  @param jobB the job jobA depends on
     *  @return true if dependency existed, false otherwise
     */
    bool removeDependency(JobPointer jobA, JobPointer jobB);
    bool removeDependency(const Dependency &dep);

    /** @brief Resolve all dependencies for a job.
     *  This method is called after the Job has been finished, or when it is deleted without being executed (performed by the
     *  destructor). The method will remove all entries stating that another Job depends on this one.
     */
    void resolveDependencies(JobPointer);

    //FIXME remove
//    /** @brief Retrieve a list of dependencies of this job. */
//    QList<JobPointer> getDependencies(JobPointer) const;

    static DependencyPolicy &instance();

    bool canRun(JobPointer) Q_DECL_OVERRIDE;

    void free(JobPointer) Q_DECL_OVERRIDE;

    void release(JobPointer) Q_DECL_OVERRIDE;

    void destructed(JobInterface *job) Q_DECL_OVERRIDE;

protected:
    /** @brief Query whether the job has an unresolved dependency.
     *  In case it does, the policy will return false from canRun().
     */
    bool hasUnresolvedDependencies(JobPointer) const;

private:
    DependencyPolicy();
    class Private;
    Private *const d;
};

}

#endif
