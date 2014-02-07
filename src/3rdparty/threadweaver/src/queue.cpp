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

#include <QtCore/QCoreApplication>
#include <QtCore/QMutex>
#include <QtCore/QVector>

#include "queue.h"
#include "weaver_p.h"

using namespace ThreadWeaver;

namespace
{
static Queue::GlobalQueueFactory *globalQueueFactory;
}

class Queue::Private
{
public:
    Private(Queue *q, QueueSignals *queue)
        : implementation(queue)
    {
        Q_ASSERT_X(qApp != 0, Q_FUNC_INFO, "Cannot create global ThreadWeaver instance before QApplication!");
        Q_ASSERT(queue);
        queue->setParent(q);
        q->connect(implementation, SIGNAL(finished()), SIGNAL(finished()));
        q->connect(implementation, SIGNAL(suspended()), SIGNAL(suspended()));
        q->connect(implementation, SIGNAL(jobDone(ThreadWeaver::JobPointer)), SIGNAL(jobDone(ThreadWeaver::JobPointer)));
    }

    QueueSignals *implementation;
    void init(QueueSignals *implementation);
};

/** @brief Construct a Queue. */
Queue::Queue(QObject *parent)
    : QueueSignals(parent)
    , d(new Private(this, new Weaver))
{
}

/** @brief Construct a Queue, specifying the QueueSignals implementation to use.
 *
 * The QueueSignals instance is usually a Weaver object, which may be customized for specific
 * application needs. The Weaver instance will take ownership of the implementation object and
 * deletes it when destructed.
 * @see Weaver
 * @see GlobalQueueFactory
 */
Queue::Queue(QueueSignals *implementation, QObject *parent)
    : QueueSignals(parent)
    , d(new Private(this, implementation))
{
}

/** @brief Destruct the Queue object.
 *
 * If the queue is not already in Destructed state, the destructor will call shutDown() to make sure
 * enqueued jobs are completed and the queue is idle.
 * The queue implementation will be destroyed.
 * @see shutDown()
 * @see ThreadWeaver::Destructed
 */
Queue::~Queue()
{
    if (d->implementation->state()->stateId() != Destructed) {
        d->implementation->shutDown();
    }
    delete d->implementation;
    delete d;
}

/** @brief Create a QueueStream to enqueue jobs into this queue. */
QueueStream Queue::stream()
{
    return QueueStream(this);
}

void Queue::shutDown()
{
    d->implementation->shutDown();
}

/** @brief Set the factory object that will create the global queue.
 *
 * Once set, the global queue factory will be deleted when the global ThreadWeaver pool is deleted.
 * The factory object needs to be set before the global ThreadWeaver pool is instantiated. Call this
 * method before Q(Core)Application is constructed. */
void Queue::setGlobalQueueFactory(Queue::GlobalQueueFactory *factory)
{
    if (globalQueueFactory) {
        delete globalQueueFactory;
    }
    globalQueueFactory = factory;
}

const State *Queue::state() const
{
    return d->implementation->state();
}

namespace
{

class StaticThreadWeaverInstanceGuard : public QObject
{
    Q_OBJECT
public:
    explicit StaticThreadWeaverInstanceGuard(QAtomicPointer<Queue> &instance, QCoreApplication *app)
        : QObject(app)
        , instance_(instance)
    {
        Q_ASSERT_X(app != 0, Q_FUNC_INFO, "Calling ThreadWeaver::Weaver::instance() requires a QCoreApplication!");
        QObject *impl = instance.load()->findChild<QueueSignals*>();
        Q_ASSERT(impl);
        impl->setObjectName(QStringLiteral("GlobalQueue"));
        qAddPostRoutine(shutDownGlobalQueue);
    }

    ~StaticThreadWeaverInstanceGuard()
    {
        instance_.fetchAndStoreOrdered(0);
        delete globalQueueFactory;
        globalQueueFactory = 0;
    }
private:
    static void shutDownGlobalQueue()
    {
        Queue::instance()->shutDown();
        Q_ASSERT(Queue::instance()->state()->stateId() == Destructed);
    }

    QAtomicPointer<Queue> &instance_;
};

}

/** @brief Access the application-global Queue.
 *
 * In some cases, the global queue is sufficient for the applications purpose. The global queue will only be
 * created if this method is actually called in the lifetime of the application.
 *
 * The Q(Core)Application object must exist when instance() is called for the first time.
 * The global queue will be destroyed when Q(Core)Application is destructed. After that, the instance() method
 * returns zero.
 */
Queue *Queue::instance()
{
    static QAtomicPointer<Queue> s_instance(globalQueueFactory
            ? globalQueueFactory->create(qApp)
            : new Queue(qApp));
    //Order is of importance here:
    //When s_instanceGuard is destructed (first, before s_instance), it sets the value of s_instance to zero. Next, qApp will delete
    //the object s_instance pointed to.
    static StaticThreadWeaverInstanceGuard *s_instanceGuard = new StaticThreadWeaverInstanceGuard(s_instance, qApp);
    Q_UNUSED(s_instanceGuard);
    Q_ASSERT_X(s_instance.load() == 0 || s_instance.load()->thread() == QCoreApplication::instance()->thread(),
               Q_FUNC_INFO,
               "The global ThreadWeaver queue needs to be instantiated (accessed first) from the main thread!");
    return s_instance.loadAcquire();
}

void Queue::enqueue(const QVector<JobPointer> &jobs)
{
    d->implementation->enqueue(jobs);
}

void Queue::enqueue(const JobPointer &job)
{
    enqueue(QVector<JobPointer>() << job);
}

bool Queue::dequeue(const JobPointer &job)
{
    return d->implementation->dequeue(job);
}

void Queue::dequeue()
{
    return d->implementation->dequeue();
}

void Queue::finish()
{
    return d->implementation->finish();
}

void Queue::suspend()
{
    return d->implementation->suspend();
}

void Queue::resume()
{
    return d->implementation->resume();
}

bool Queue::isEmpty() const
{
    return d->implementation->isEmpty();
}

bool Queue::isIdle() const
{
    return d->implementation->isIdle();
}

int Queue::queueLength() const
{
    return d->implementation->queueLength();
}

void Queue::setMaximumNumberOfThreads(int cap)
{
    d->implementation->setMaximumNumberOfThreads(cap);
}

int Queue::currentNumberOfThreads() const
{
    return d->implementation->currentNumberOfThreads();
}

int Queue::maximumNumberOfThreads() const
{
    return d->implementation->maximumNumberOfThreads();
}

void Queue::requestAbort()
{
    d->implementation->requestAbort();
}

void Queue::reschedule()
{
    d->implementation->reschedule();
}

#include "queue.moc"
