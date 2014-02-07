/* -*- C++ -*-

   A decorator to make jobs into QObjects in ThreadWeaver.

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

#ifndef THREADWEAVER_QOBJECTDECORATOR_H
#define THREADWEAVER_QOBJECTDECORATOR_H

#include <QtCore/QObject>

#include "threadweaver_export.h"
#include "iddecorator.h"

namespace ThreadWeaver
{

class Collection;
class Sequence;

class THREADWEAVER_EXPORT QObjectDecorator : public QObject, public IdDecorator
{
    Q_OBJECT
public:
    explicit QObjectDecorator(JobInterface *decoratee, QObject *parent = 0);
    explicit QObjectDecorator(JobInterface *decoratee, bool autoDelete, QObject *parent = 0);

Q_SIGNALS:
    /** This signal is emitted when this job is being processed by a thread. */
    void started(ThreadWeaver::JobPointer);
    /** This signal is emitted when the job has been finished (no matter if it succeeded or not). */
    void done(ThreadWeaver::JobPointer);
    /** This job has failed.
     *
     * This signal is emitted when success() returns false after the job is executed. */
    void failed(ThreadWeaver::JobPointer);

protected:
    void defaultBegin(JobPointer job, Thread *thread) Q_DECL_OVERRIDE;
    void defaultEnd(JobPointer job, Thread *thread) Q_DECL_OVERRIDE;
};

typedef QSharedPointer<QObjectDecorator> QJobPointer;

}

#endif // THREADWEAVER_QOBJECTDECORATOR_H
