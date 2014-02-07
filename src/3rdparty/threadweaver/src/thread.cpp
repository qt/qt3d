/* -*- C++ -*-

   This file implements the Thread class.

   Thread is not a part of the public interface of the ThreadWeaver library.

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

   $Id: Thread.cpp 25 2005-08-14 12:41:38Z mirko $
*/

#include "thread.h"

#include <QtCore/QMutex>
#include <QtCore/QDebug>
#include <QPointer>
#include <QtCore/QCoreApplication>

#include "threadweaver.h"
#include "weaver_p.h"
#include "job.h"
#include "debuggingaids.h"

using namespace ThreadWeaver;

class Thread::Private
{
public:
    explicit Private(Weaver *theParent)
        : parent(theParent)
        , id(makeId())
        , job(0)
    {
        Q_ASSERT(parent);
    }

    Weaver *parent;
    const unsigned int id;
    JobPointer job;
    QMutex mutex;

    static unsigned int makeId()
    {
        static QAtomicInt s_id(1);
        return s_id.fetchAndAddRelease(1);
    }
};

Thread::Thread(Weaver *parent)
    : QThread() // no parent, because the QObject hierarchy of this thread
    // does not have a parent (see QObject::pushToThread)
    , d(new Private(parent))
{
    const QString queueName = parent->objectName().isEmpty()
                              ? QString::fromLatin1("Queue(0x%1)").arg(quintptr(parent), 0, 16, QChar::fromLatin1('0'))
                              : parent->objectName();
    setObjectName(QString::fromLatin1("%1[%2]").arg(queueName).arg(QString::number(id()), 2, QChar::fromLatin1('0')));
}

Thread::~Thread()
{
    delete d;
}

unsigned int Thread::id()
{
    return d->id; //id is const
}

void Thread::run()
{
    Q_ASSERT(d->parent);
    Q_ASSERT(QCoreApplication::instance() != 0);
    d->parent->threadEnteredRun(this);

    emit started(this);
    debug(3, "Thread::run [%u]: running.\n", id());

    bool wasBusy = false;
    while (true) {
        debug(3, "Thread::run [%u]: trying to execute the next job.\n", id());

        // the assignment is intentional: newJob needs to go out of scope at the end of the if statement
        if (JobPointer newJob = d->parent->applyForWork(this, wasBusy)) {
            QMutexLocker l(&d->mutex); Q_UNUSED(l);
            d->job = newJob;
        } else {
            break;
        }

        wasBusy = true;

        emit jobStarted(d->job, this);
        d->job->execute(d->job, this);
        emit jobDone(d->job);

        QMutexLocker l(&d->mutex); Q_UNUSED(l);
        d->job.clear();
    }
    debug(3, "Thread::run [%u]: exiting.\n", id());
}

void Thread::requestAbort()
{
    QMutexLocker l(&d->mutex); Q_UNUSED(l);
    if (d->job) {
        d->job->requestAbort();
    }
}

