/* -*- C++ -*-

   This file declares the Sequence class.

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

   $Id: DebuggingAids.h 30 2005-08-16 16:16:04Z mirko $
*/

#ifndef JOBSEQUENCE_H
#define JOBSEQUENCE_H

#include "collection.h"

namespace ThreadWeaver
{

/** @brief A Sequence is a vector of Jobs that will be executed in a sequence.
 *
 * It is implemented by automatically creating the necessary dependencies between the Jobs in the sequence.
 *
 * Sequence provides a handy cleanup and unwind mechanism: the stop() slot. If it is called, the processing
 * of the sequence will stop, and all its remaining Jobs will be dequeued.
 * A Sequence is the first element of itself. */
class THREADWEAVER_EXPORT Sequence : public Collection
{
public:
    explicit Sequence();

protected:
    void elementFinished(JobPointer job, Thread *thread) Q_DECL_OVERRIDE;
    void enqueueElements() Q_DECL_OVERRIDE;

private:
    class Private;
    Private *const d;
};

}

#endif
