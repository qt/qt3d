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

#ifndef THREADWEAVER_QUEUESIGNALS_H
#define THREADWEAVER_QUEUESIGNALS_H

#include <QtCore/QObject>
#include "queueinterface.h"

namespace ThreadWeaver
{

/** @brief QueueSignals declares the Qt signals shared by the Queue and Weaver classes. */
class QueueSignals : public QObject, public QueueInterface
{
    Q_OBJECT
public:
    explicit QueueSignals(QObject *parent = 0);

Q_SIGNALS:
    /** @brief Emitted when the Queue has completed all jobs currently queued.
     *
     * The Queue emits finished() when the the job queue is empty, and the last job currently processed by a worker threads was
     * completed. Beware that if multiple jobs are enqueued repeatedly one by one, this signal might be emitted multiple times, because the
     * queued jobs where processed before new ones could be queued. To avoid this, queue all relevant jobs in a single operation,
     * using for example a QueueStream or a Collection.
     */
    void finished();

    /** @brief The Queue has been suspended.
     *
     * When the Queue is suspended, worker threads will not be assigned new jobs to process. Jobs waiting in the queue will not be
     * started until processing is resumed. When suspend() is called, the worker threads will continue to process the job currently
     * assigned to them. When the last thread finishes it's current assignment, suspended() is emitted.
     *
     * @see suspend()
     */
    void suspended();

    /** @brief A Job has been completed.
     *
     * This signal is emitted when a job is finished by a worker thread.
     */
    void jobDone(ThreadWeaver::JobPointer);

    /** @brief Emitted when the processing state of the Queue has changed. */
    void stateChanged(ThreadWeaver::State*);
};

}

#endif // THREADWEAVER_QUEUESIGNALS_H
