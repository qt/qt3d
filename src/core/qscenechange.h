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

#ifndef QT3D_QSCENECHANGE_H
#define QT3D_QSCENECHANGE_H

#include <Qt3DCore/qt3dcore_global.h>
#include <QSharedPointer>

QT_BEGIN_NAMESPACE

namespace Qt3D {

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

class QNode;
class QObservableInterface;
class QSceneChangePrivate;

class QT3DCORESHARED_EXPORT QSceneChange
{
public:
    enum Priority {
        High,
        Standard,
        Low
    };

    enum ObservableType {
        Observable,
        Node
    };

    union SubjectUnion {
        QObservableInterface *m_observable;
        QNode *m_node;
    };

    QSceneChange(ChangeFlag type, QObservableInterface *observable, Priority priority = Standard);
    QSceneChange(ChangeFlag type, QNode *node, Priority priority = Standard);
    virtual ~QSceneChange();

    ChangeFlag type() const;
    qint64 timestamp() const;
    QSceneChange::Priority priority() const;
    QSceneChange::ObservableType observableType() const;
    QSceneChange::SubjectUnion subject() const;

protected:
    Q_DECLARE_PRIVATE(QSceneChange)
    QSceneChangePrivate *d_ptr;
    QSceneChange(QSceneChangePrivate &dd);

    // TODO: add timestamp from central clock and priority level
    // These can be used to resolve any conflicts between events
    // posted from different aspects
};

typedef QSharedPointer<QSceneChange> QSceneChangePtr;

} // namespace Qt3D

QT_END_NAMESPACE

#endif // QT3D_QSCENECHANGE_H
