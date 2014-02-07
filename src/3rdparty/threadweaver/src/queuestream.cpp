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

#include <QVector>

#include "queuestream.h"
#include "queue.h"
#include "queueing.h"

namespace ThreadWeaver
{

class QueueStream::Private
{
public:
    Private(Queue *w) : weaver(w) {}
    Queue *weaver;
    QVector<JobPointer> jobs;
};

QueueStream::QueueStream(Queue *queue)
    : d(new Private(queue))
{
    Q_ASSERT(queue);
}

QueueStream::~QueueStream()
{
    flush();
    delete d;
}

void ThreadWeaver::QueueStream::add(const ThreadWeaver::JobPointer &job)
{
    d->jobs.append(job);
}

void QueueStream::flush()
{
    if (d->jobs.isEmpty()) {
        return;
    }
    Q_ASSERT(d->weaver);
    d->weaver->enqueue(d->jobs);
    d->jobs.clear();
}

QueueStream &QueueStream::operator<<(const JobPointer &job)
{
    add(job);
    return *this;
}

QueueStream &QueueStream::operator<<(JobInterface *job)
{
    add(make_job(job));
    return *this;
}

QueueStream &QueueStream::operator<<(Job &job)
{
    add(make_job_raw(&job));
    return *this;
}

/** @brief Return a stream the enqueues jobs in the ThreadWeaver global queue.
 *  Using this is synonymous to Weaver::instance()::stream(). */
QueueStream stream()
{
    return QueueStream(Queue::instance());
}

}

