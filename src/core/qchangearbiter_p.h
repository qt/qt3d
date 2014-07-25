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

#ifndef QT3D_QCHANGEARBITER_P_H
#define QT3D_QCHANGEARBITER_P_H

#include <private/qobject_p.h>
#include <QMutex>
#include <QThreadStorage>

QT_BEGIN_NAMESPACE

namespace Qt3D {

class QNode;
class QChangeArbiter;
class QObservableInterface;
class QJobManagerInterface;
class QSceneObserverInterface;

typedef QVector<QSceneChangePtr> ChangeQueue;
typedef QPair<ChangeFlags, QObserverInterface *> QObserverPair;
typedef QList<QObserverPair> QObserverList;

class QChangeArbiterPrivate : public QObjectPrivate
{
public:
    QChangeArbiterPrivate(QChangeArbiter *qq);

    Q_DECLARE_PUBLIC(QChangeArbiter)

    QMutex m_mutex;
    QJobManagerInterface *m_jobManager;

    // The lists of observers indexed by observable. We maintain two
    // distinct hashes:
    //
    // m_aspectObservations is for observables owned by aspects
    // m_nodeObservations is for observables in the main thread's object tree
    //
    // We keep these distinct because we do not manage the main thread which means
    // the mechanisms for working with objects there is different.
    QHash<QObservableInterface *, QObserverList> m_aspectObservations;
    QHash<QUuid, QObserverList> m_nodeObservations;
    QList<QSceneObserverInterface *> m_sceneObservers;
    QHash<QUuid, QNode *> m_idToNodeObservable;

    // Each thread has a TLS ChangeQueue so we never need to lock whilst
    // receiving a QSceneChange.
    QThreadStorage<ChangeQueue *> m_tlsChangeQueue;

    // We store a list of the ChangeQueue's from each thread. This will only
    // be accessed from the aspect thread during the syncChanges() phase.
    QList<ChangeQueue *> m_changeQueues;
    QList<ChangeQueue *> m_lockingChangeQueues;
};

} // Qt3D

QT_END_NAMESPACE

#endif // QT3D_QCHANGEARBITER_P_H
