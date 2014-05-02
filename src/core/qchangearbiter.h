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

#ifndef QT3D_QCHANGEARBITER_H
#define QT3D_QCHANGEARBITER_H

#include <Qt3DCore/qt3dcore_global.h>
#include <QObject>
#include <QDateTime>
#include <QDebug>
#include <QFlags>
#include <QMutex>
#include <QReadWriteLock>
#include <QSharedPointer>
#include <QThreadStorage>
#include <QVariant>
#include <QVector>
#include <QPair>

QT_BEGIN_NAMESPACE

namespace Qt3D {

class Component;

enum ChangeFlag {
    NodeCreated             = 0x00000001,
    NodeAboutToBeDeleted    = 0x00000002,
    NodeDeleted             = 0x00000004,
    NodeStatus              = 0x00000008,
    ComponentAdded          = 0x00000010,
    ComponentRemoved        = 0x00000020,
    ComponentUpdated        = 0x00000040,
    AllChanges              = 0x00000FFF
};
Q_DECLARE_FLAGS(ChangeFlags, ChangeFlag)
Q_DECLARE_OPERATORS_FOR_FLAGS(ChangeFlags)


class QObserverInterface;
class QObservableInterface;

class QT3DCORESHARED_EXPORT QSceneChange
{
public:
    enum Priority {
        High,
        Standard,
        Low
    };

    QSceneChange(ChangeFlag type, QObservableInterface *observable, Priority priority = Standard)
        : m_type(type),
          m_priority(priority),
          m_timestamp(QDateTime::currentMSecsSinceEpoch())
    {
        m_subject.m_observable = observable;
        m_subjectType = ObservableType;
    }

    QSceneChange(ChangeFlag type, Component *component, Priority priority = Standard)
        : m_type(type),
          m_priority(priority),
          m_timestamp(QDateTime::currentMSecsSinceEpoch())
    {
        m_subject.m_component = component;
        m_subjectType = ComponentType;
    }

    union {
        QObservableInterface *m_observable;
        Component *m_component;
    } m_subject;

    enum ObservableType {
        ObservableType,
        ComponentType
    } m_subjectType;

    ChangeFlag m_type;
    Priority m_priority;
    qint64 m_timestamp;

    // TODO: add timestamp from central clock and priority level
    // These can be used to resolve any conflicts between events
    // posted from different aspects
};

typedef QSharedPointer<QSceneChange> QSceneChangePtr;

class QT3DCORESHARED_EXPORT QScenePropertyChange : public QSceneChange
{
public:
    QScenePropertyChange(ChangeFlag type, QObservableInterface *subject, Priority priority = Standard)
        : QSceneChange(type, subject, priority)
    {
    }

    QScenePropertyChange(ChangeFlag type, Component *component, Priority priority = Standard)
        : QSceneChange(type, component, priority)
    {
    }

    QByteArray m_propertyName;
    QVariant m_value;
};

typedef QSharedPointer<QScenePropertyChange> QScenePropertyChangePtr;


class QT3DCORESHARED_EXPORT QObservableInterface
{
public:
    virtual ~QObservableInterface() {}

    virtual void registerObserver(QObserverInterface *observer) = 0;
    virtual void unregisterObserver(QObserverInterface *observer) = 0;

protected:
    virtual void notifyObservers(const QSceneChangePtr &e) = 0;
};


class QT3DCORESHARED_EXPORT QObservable : public QObservableInterface
{
public:
    QObservable();

    void registerObserver(QObserverInterface *observer) Q_DECL_OVERRIDE;
    void unregisterObserver(QObserverInterface *observer) Q_DECL_OVERRIDE;

protected:
    void notifyObservers(const QSceneChangePtr &e) Q_DECL_OVERRIDE;

    const QList<QObserverInterface *> &observers() const { return m_observers; }

private:
    QList<QObserverInterface *> m_observers;
    QReadWriteLock m_lock;
};


class QObserverInterface
{
public:
    virtual ~QObserverInterface() {}

    virtual void sceneChangeEvent(const QSceneChangePtr &e) = 0;
};


class QJobManagerInterface;

class QT3DCORESHARED_EXPORT QChangeArbiter : public QObject,
                                             public QObserverInterface
{
    Q_OBJECT
public:
    explicit QChangeArbiter(QObject *parent = 0);

    void initialize(Qt3D::QJobManagerInterface *jobManager);

    void syncChanges();

    void registerObserver(QObserverInterface *observer,
                          QObservableInterface *subject,
                          ChangeFlags changeFlags = AllChanges);

    void registerObserver(QObserverInterface *observer,
                          Component *component,
                          ChangeFlags changeFlags = AllChanges);

    void unregisterObserver(QObserverInterface *observer,
                            QObservableInterface *subject);

    void unregisterObserver(QObserverInterface *observer,
                            Component *subject);

    void sceneChangeEvent(const QSceneChangePtr &e) Q_DECL_OVERRIDE;                 // QObserverInterface impl
    void sceneChangeEventWithLock(const QSceneChangePtr &e);

    static void createUnmanagedThreadLocalChangeQueue(void *changeArbiter);
    static void destroyUnmanagedThreadLocalChangeQueue(void *changeArbiter);
    static void createThreadLocalChangeQueue(void *changeArbiter);
    static void destroyThreadLocalChangeQueue(void *changeArbiter);

private:
    typedef QVector<QSceneChangePtr> ChangeQueue;

    void distributeQueueChanges(ChangeQueue *queue);

    QMutex m_mutex;
    QJobManagerInterface *m_jobManager;

    typedef QPair<ChangeFlags, QObserverInterface *> QObserverPair;
    typedef QList<QObserverPair> QObserverList;
    QHash<QObservableInterface *, QObserverList> m_observations;
    QHash<Component *, QObserverList> m_componentObservations;

    // Each thread has a TLS ChangeQueue so we never need to lock whilst
    // receiving a QSceneChange.
    QThreadStorage<ChangeQueue *> m_tlsChangeQueue;

    // We store a list of the ChangeQueue's from each thread. This will only
    // be accesses from the aspect thread during the syncChanges() phase.
    QList<ChangeQueue *> m_changeQueues;
    QList<ChangeQueue *> m_lockingChangeQueues;
};

} // namespace Qt3D

QT_END_NAMESPACE

#endif // QT3D_QCHANGEARBITER_H
