/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtQuick3D module of the Qt Toolkit.
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
#include <QtCore/qmutex.h>
#include <QtCore/qcoreapplication.h>

QT_BEGIN_NAMESPACE

#if !defined(Q_MOC_RUN)

class Q_OPENGL_EXPORT QGLSignalProxy : public QObject
{
    Q_OBJECT
public:
    QGLSignalProxy() : QObject() {}
    void emitAboutToDestroyContext(const QGLContext *context) {
        emit aboutToDestroyContext(context);
    }
    static QGLSignalProxy *instance();
Q_SIGNALS:
    void aboutToDestroyContext(const QGLContext *context);
};

#endif

class QGLContextInfo
{
public:
    QGLContextInfo(const QGLContext *ctx) : m_context(ctx), m_resources(0) {}
    ~QGLContextInfo();

    const QGLContext *m_context;
    QGLSharedResource *m_resources;
};

QGLContextInfo::~QGLContextInfo()
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

class QGLContextManager : public QObject
{
    Q_OBJECT
public:
    QGLContextManager(QObject *parent = 0);
    ~QGLContextManager();

    QMutex managerLock;

    QGLContextInfo *contextInfo(const QGLContext *ctx);

private Q_SLOTS:
    void aboutToDestroyContext(const QGLContext *ctx);

private:
    QList<QGLContextInfo *> m_contexts;
};

Q_GLOBAL_STATIC(QGLContextManager, qt_gl_context_manager)

QGLContextManager::QGLContextManager(QObject *parent)
    : QObject(parent)
{
    QGLSignalProxy *proxy = QGLSignalProxy::instance();
    QThread *mainThread = qApp->thread();
    if (thread() != mainThread) {
        // The manager and signal proxy have been created for the first
        // time in a background thread.  For safety, move both objects
        // to the main thread.
        moveToThread(mainThread);
        proxy->moveToThread(mainThread);
    }
    connect(proxy, SIGNAL(aboutToDestroyContext(const QGLContext *)),
            this, SLOT(aboutToDestroyContext(const QGLContext *)));
}

QGLContextManager::~QGLContextManager()
{
    QMutexLocker locker(&managerLock);
    qDeleteAll(m_contexts);
}

QGLContextInfo *QGLContextManager::contextInfo(const QGLContext *ctx)
{
    QGLContextInfo *info;
    for (int index = 0; index < m_contexts.size(); ++index) {
        info = m_contexts[index];
        if (info->m_context == ctx)
            return info;
    }
    info = new QGLContextInfo(ctx);
    m_contexts.append(info);
    return info;
}

Q_OPENGL_EXPORT const QGLContext *qt_gl_transfer_context(const QGLContext *);

void QGLContextManager::aboutToDestroyContext(const QGLContext *ctx)
{
    QMutexLocker locker(&managerLock);
    int index = 0;
    while (index < m_contexts.size()) {
        QGLContextInfo *info = m_contexts[index];
        if (info->m_context == ctx) {
            const QGLContext *transfer = qt_gl_transfer_context(ctx);
            if (transfer) {
                // Transfer ownership to another context in the same sharing
                // group.  This may result in multiple QGLContextInfo objects
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

const QGLContext *QGLSharedResource::context() const
{
    // Hope that the context will not be destroyed in another thread
    // while we are doing this so we don't have to acquire the lock.
    return m_contextInfo ? m_contextInfo->m_context : 0;
}

void QGLSharedResource::attach(const QGLContext *context, GLuint id)
{
    Q_ASSERT(!m_contextInfo);
    QGLContextManager *manager = qt_gl_context_manager();
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
    QGLContextManager *manager = qt_gl_context_manager();
    const QGLContext *owner = 0;
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
        QGLContext *currentContext = const_cast<QGLContext *>(QGLContext::currentContext());
        QGLContext *oldContext;
        QGLContext *doneContext;
        if (currentContext != owner && !QGLContext::areSharing(owner, currentContext)) {
            oldContext = currentContext;
            doneContext = const_cast<QGLContext *>(owner);
            doneContext->makeCurrent();
        } else {
            oldContext = 0;
            doneContext = 0;
        }
        m_destroyFunc(id);
        if (oldContext)
            oldContext->makeCurrent();
        else if (!currentContext && doneContext)
            doneContext->doneCurrent();
    }
}

QT_END_NAMESPACE

#include "qglsharedresource.moc"
