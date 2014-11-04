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

#ifndef QT3D_QNODE_P_H
#define QT3D_QNODE_P_H

#include <private/qobject_p.h>
#include <Qt3DCore/qt3dcore_global.h>
#include <Qt3DCore/qnode.h>
#include <Qt3DCore/private/qobservableinterface_p.h>
#include <Qt3DCore/private/qchangearbiter_p.h>
#include "propertychangehandler_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3D {

class QNode;
class QAspectEngine;

class QT3DCORESHARED_EXPORT QNodePrivate : public QObjectPrivate, public QObservableInterface
{
public:
    QNodePrivate(QNode *qq);

    // Clone should only be made in the main thread
    QNode *clone();

    void setScene(QSceneInterface *scene);
    QSceneInterface *scene() const;

    void setArbiter(QChangeArbiter *arbiter) Q_DECL_OVERRIDE;

    void notifyPropertyChange(const char *name, const QVariant &value);
    virtual void notifyObservers(const QSceneChangePtr &change);

    void insertTree(QNode *treeRoot, int depth = 0);

    Q_DECLARE_PUBLIC(QNode)

    // For now this just protects access to the m_changeArbiter.
    // Later on we may decide to extend support for multiple observers.
    QChangeArbiter *m_changeArbiter;
    QSceneInterface *m_scene;
    mutable QNodeUuid m_uuid;
    bool m_blockNotifications;

    static QNodePrivate *get(QNode *q);
    static void nodePtrDeleter(QNode *q);

private:
    void addChild(QNode *childNode);
    void removeChild(QNode *childNode);
    void removeAllChildren();
    void registerNotifiedProperties();
    void unregisterNotifiedProperties();
    void propertyChanged(int propertyIndex);

    friend class PropertyChangeHandler<QNodePrivate>;
    bool m_propertyChangesSetup;
    PropertyChangeHandler<QNodePrivate> m_signals;

    static QHash<QNodeUuid, QNode *> m_clonesLookupTable;
};

} // namespace Qt3D

QT_END_NAMESPACE

#endif // QT3D_NODE_P_H
