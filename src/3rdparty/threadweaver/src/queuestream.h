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

#ifndef QUEUESTREAM_H
#define QUEUESTREAM_H

#include "jobinterface.h"
#include "threadweaver_export.h"

namespace ThreadWeaver
{

class Queue;
class Job;

/** @brief QueueStream implements a stream based API to access ThreadWeaver queues. */
class THREADWEAVER_EXPORT QueueStream
{
public:
    explicit QueueStream(Queue *queue);
    ~QueueStream();
    void add(const JobPointer &job);
    void flush();

    QueueStream &operator<<(const JobPointer &job);
    QueueStream &operator<<(JobInterface *job);
    //FIXME try with QObjectDecorator (JobInterface&)
    QueueStream &operator<<(Job &job);

private:
    class Private;
    Private *const d;
};

QueueStream THREADWEAVER_EXPORT stream();

}

#endif // QUEUESTREAM_H
