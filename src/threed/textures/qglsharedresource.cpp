/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qglsharedresource_p.h"

#include <QMutex>
#include <QGuiApplication>
#include <QOpenGLContext>

QT_BEGIN_NAMESPACE

class QOpenGLContextInfo
{
public:
    QOpenGLContextInfo(QOpenGLContext *ctx) : m_context(ctx), m_resources(0) {}
    ~QOpenGLContextInfo();

    QOpenGLContext *m_context;
    QGLSharedResource *m_resources;
};

QOpenGLContextInfo::~QOpenGLContextInfo()
{
    // Detach this information block from all of the shared resources
    // that used to be owned by it.
    QGLSharedResource *resource = m_resources;
    while (resource != 0) {
        resource->m_contextInfo = 0;
        resource->m_id = 0;
        resource = resource->m_next;
    }
}

class QOpenGLContextManager : public QObject
{
    Q_OBJECT
public:
    QOpenGLContextManager(QObject *parent = 0);
    ~QOpenGLContextManager();

    QMutex managerLock;

    QOpenGLContextInfo *contextInfo(QOpenGLContext *ctx);

private Q_SLOTS:
    void aboutToDestroyContext();

private:
    QList<QOpenGLContextInfo *> m_contexts;
};

Q_GLOBAL_STATIC(QOpenGLContextManager, qt_gl_context_manager)

QOpenGLContextManager::QOpenGLContextManager(QObject *parent)
    : QObject(parent)
{
}

QOpenGLContextManager::~QOpenGLContextManager()
{
    QMutexLocker locker(&managerLock);
    qDeleteAll(m_contexts);
}

QOpenGLContextInfo *QOpenGLContextManager::contextInfo(QOpenGLContext *ctx)
{
    QOpenGLContextInfo *info = 0;
    if (ctx)
    {
        for (int index = 0; index < m_contexts.size(); ++index)
        {
            info = m_contexts[index];
            if (info->m_context == ctx)
                return info;
        }
        info = new QOpenGLContextInfo(ctx);
        m_contexts.append(info);
        connect(ctx, SIGNAL(destroyed()), this, SLOT(aboutToDestroyContext()));
    }
    return info;
}

void QOpenGLContextManager::aboutToDestroyContext()
{
    QOpenGLContext *ctx = qobject_cast<QOpenGLContext *>(sender());
    Q_ASSERT_X(ctx, Q_FUNC_INFO, "aboutToDestroy signal not from context");
    QMutexLocker locker(&managerLock);
    int index = 0;
    while (index < m_contexts.size()) {
        QOpenGLContextInfo *info = m_contexts[index];
        if (info->m_context == ctx) {
            QOpenGLContext *transfer = ctx->shareContext();
            if (transfer) {
                // Transfer ownership to another context in the same sharing
                // group.  This may result in multiple QOpenGLContextInfo objects
                // for the same context, which is ok.
                info->m_context = transfer;
            } else {
                // All contexts in the sharing group have been deleted,
                // so detach all of the shared resources.
                m_contexts.removeAt(index);
                delete info;
                continue;
            }
        }
        ++index;
    }
}

QOpenGLContext *QGLSharedResource::context() const
{
    // Hope that the context will not be destroyed in another thread
    // while we are doing this so we don't have to acquire the lock.
    return m_contextInfo ? m_contextInfo->m_context : 0;
}

void QGLSharedResource::attach(QOpenGLContext *context, GLuint id)
{
    Q_ASSERT(!m_contextInfo);
    QOpenGLContextManager *manager = qt_gl_context_manager();
    QMutexLocker locker(&(manager->managerLock));
    m_contextInfo = manager->contextInfo(context);
    m_id = id;
    m_next = m_contextInfo->m_resources;
    m_prev = 0;
    if (m_contextInfo->m_resources)
        m_contextInfo->m_resources->m_prev = this;
    m_contextInfo->m_resources = this;
}

void QGLSharedResource::destroy()
{
    // Detach this resource from the context information block.
    QOpenGLContextManager *manager = qt_gl_context_manager();
    QOpenGLContext *owner = 0;
    GLuint id = 0;
    manager->managerLock.lock();
    if (m_contextInfo) {
        if (m_next)
            m_next->m_prev = m_prev;
        if (m_prev)
            m_prev->m_next = m_next;
        else
            m_contextInfo->m_resources = m_next;
        owner = m_contextInfo->m_context;
        id = m_id;
    }
    m_contextInfo = 0;
    m_id = 0;
    m_next = 0;
    m_prev = 0;
    manager->managerLock.unlock();

    // Switch back to the owning context temporarily and delete the id.
    if (owner && id) {
        QOpenGLContext *currentContext = QOpenGLContext::currentContext();
        QSurface *surf = currentContext->surface();
        QOpenGLContext *oldContext;
        QOpenGLContext *doneContext;
        if (currentContext != owner && !QOpenGLContext::areSharing
                (const_cast<QOpenGLContext *>(owner), currentContext)) {
            oldContext = currentContext;
            doneContext = owner;
            doneContext->makeCurrent(surf);
        } else {
            oldContext = 0;
            doneContext = 0;
        }
        m_destroyFunc(id);
        if (oldContext)
            oldContext->makeCurrent(surf);
        else if (!currentContext && doneContext)
            doneContext->doneCurrent();
    }
}

QT_END_NAMESPACE

#include "qglsharedresource.moc"
