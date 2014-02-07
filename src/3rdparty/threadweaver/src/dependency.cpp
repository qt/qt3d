/* -*- C++ -*-

   A dependency between jobs.

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

#include "dependency.h"
#include "jobinterface.h"
#include "managedjobpointer.h"

namespace ThreadWeaver
{

Dependency::Dependency(const JobPointer &dependent, const JobPointer &dependee)
    : m_dependent(dependent)
    , m_dependee(dependee)
{
}

Dependency::Dependency(JobInterface *dependent, JobInterface *dependee)
    : m_dependent(ManagedJobPointer<JobInterface>(dependent))
    , m_dependee(ManagedJobPointer<JobInterface>(dependee))
{
}

Dependency::Dependency(const JobPointer &dependent, JobInterface *dependee)
    : m_dependent(dependent)
    , m_dependee(ManagedJobPointer<JobInterface>(dependee))
{
}

Dependency::Dependency(JobInterface *dependent, const JobPointer &dependee)
    : m_dependent(ManagedJobPointer<JobInterface>(dependent))
    , m_dependee(dependee)
{
}

JobPointer Dependency::dependent() const
{
    return m_dependent;
}

JobPointer Dependency::dependee() const
{
    return m_dependee;
}

}
