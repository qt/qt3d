/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qaspectengine.h"

#include "qaspectthread.h"
#include "qaspectmanager.h"
#include "qchangearbiter.h"

#include <QDebug>
#include <QMetaObject>
#include <QMutexLocker>

namespace Qt3D {

QAspectEngine::QAspectEngine(QObject *parent)
    : QObject(parent)
    , m_aspectThread(new QAspectThread(this))
{
    qDebug() << Q_FUNC_INFO;
    qRegisterMetaType<QWaitCondition *>();
    m_aspectThread->waitForStart(QThread::HighestPriority);
}

void QAspectEngine::initialize()
{
    QChangeArbiter *arbiter = m_aspectThread->aspectManager()->changeArbiter();
    QChangeArbiter::createUnmanagedThreadLocalChangeQueue(arbiter);
}

void QAspectEngine::shutdown()
{
}

void QAspectEngine::setWindow(QWindow *window)
{
    QMetaObject::invokeMethod(m_aspectThread->aspectManager(), "setWindow", Q_ARG(QWindow *, window));
}

void QAspectEngine::setRoot(QObject *rootObject)
{
    QMutexLocker locker(&m_mutex);
    QMetaObject::invokeMethod(m_aspectThread->aspectManager(), "setRoot", Q_ARG(QObject *, rootObject), Q_ARG(QWaitCondition *, &m_waitCondition));

    qDebug() << "Putting main thread to sleep whilst aspects build their local scenes...";
    m_waitCondition.wait(&m_mutex);
    qDebug() << "Main thread is now awake again";
}

} // namespace Qt3D
