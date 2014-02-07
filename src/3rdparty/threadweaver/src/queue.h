/* -*- C++ -*-

   The Queue class in ThreadWeaver.

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

#ifndef THREADWEAVER_QUEUE_H
#define THREADWEAVER_QUEUE_H

#include <QtCore/QObject>

#include "queuesignals.h"
#include "queuestream.h"

namespace ThreadWeaver
{

class Job;
class State;

/** @brief Queue implements a ThreadWeaver job queue.
 *
 * Queues process jobs enqueued in them by automatically assigning them to worker threads they manage.
 * Applications using ThreadWeaver can make use of a global Queue which is instantiated on demand, or
 * create multiple queues as needed. A job assigned to a queue will be processed by that specific queue.
 *
 * Worker threads are created by the queues as needed. To create a customized global queue,
 * see GlobalQueueFactory.
 *
 * @see GlobalQueueFactory
 * @see Queue::enqueue()
 * @see Queue::instance()
 */
class THREADWEAVER_EXPORT Queue : public QueueSignals
{
    Q_OBJECT
public:
    explicit Queue(QObject *parent = 0);
    explicit Queue(QueueSignals *implementation, QObject *parent = 0);
    virtual ~Queue();

    QueueStream stream();

    const State *state() const;

    void setMaximumNumberOfThreads(int cap) Q_DECL_OVERRIDE;
    int maximumNumberOfThreads() const Q_DECL_OVERRIDE;
    int currentNumberOfThreads() const Q_DECL_OVERRIDE;

    static ThreadWeaver::Queue *instance();
    void enqueue(const QVector<JobPointer> &jobs) Q_DECL_OVERRIDE;
    void enqueue(const JobPointer &job);
    bool dequeue(const JobPointer &) Q_DECL_OVERRIDE;
    void dequeue() Q_DECL_OVERRIDE;
    void finish() Q_DECL_OVERRIDE;
    void suspend() Q_DECL_OVERRIDE;
    void resume() Q_DECL_OVERRIDE;
    bool isEmpty() const Q_DECL_OVERRIDE;
    bool isIdle() const Q_DECL_OVERRIDE;
    int queueLength() const Q_DECL_OVERRIDE;
    void requestAbort() Q_DECL_OVERRIDE;
    void reschedule() Q_DECL_OVERRIDE;
    void shutDown() Q_DECL_OVERRIDE;

    /** @brief Interface for the global queue factory. */
    struct GlobalQueueFactory {
        virtual ~GlobalQueueFactory() {}
        virtual Queue *create(QObject *parent) = 0;

    };
    static void setGlobalQueueFactory(GlobalQueueFactory *factory);
private:
    class Private;
    Private *const d;
};

}

#endif // THREADWEAVER_QUEUE_H
