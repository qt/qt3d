/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qaspectthread.h"

#include "qaspectmanager_p.h"

#include <Qt3DCore/private/corelogging_p.h>
#include <QMutexLocker>

QT_BEGIN_NAMESPACE

namespace Qt3D {

QAspectThread::QAspectThread(QObject *parent)
    : QThread(parent),
      m_aspectManager(Q_NULLPTR),
      m_semaphore(0)
{
    qCDebug(Aspects) << Q_FUNC_INFO;
}

void QAspectThread::waitForStart(Priority priority)
{
    qCDebug(Aspects) << "Starting QAspectThread and going to sleep until it is ready for us...";
    start(priority);
    m_semaphore.acquire();
    qCDebug(Aspects) << "QAspectThead is now ready & calling thread is now awake again";
}

void QAspectThread::run()
{
    qCDebug(Aspects) << "Entering void QAspectThread::run()";

    m_aspectManager = new QAspectManager;

    // Load and initialize the aspects and any other core services
    // Done before releasing condition to make sure that Qml Components
    // Are exposed prior to Qml Engine source being set
    m_aspectManager->initialize();

    // Wake up the calling thread now that our worker objects are ready for action
    m_semaphore.release();

    // Enter the main loop
    m_aspectManager->exec();

    // Clean up
    m_aspectManager->shutdown();

    // Delete the aspect manager while we're still in the thread
    delete m_aspectManager;

    qCDebug(Aspects) << "Exiting void QAspectThread::run()";
}

} // namespace Qt3D

QT_END_NAMESPACE
