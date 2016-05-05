/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QT3DCORE_QSCENECHANGE_H
#define QT3DCORE_QSCENECHANGE_H

#include <Qt3DCore/qt3dcore_global.h>
#include <QSharedPointer>
#include <Qt3DCore/qnodeid.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

enum ChangeFlag {
    NodeCreated             = 1 << 0,
    NodeDeleted             = 1 << 1,
    NodeUpdated             = 1 << 2,
    NodeAdded               = 1 << 3,
    NodeRemoved             = 1 << 4,
    ComponentAdded          = 1 << 5,
    ComponentRemoved        = 1 << 6,
    AllChanges              = 0xFFFFFFFF
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

    enum SenderType {
        BackendNode,
        Node
    };

    QSceneChange(ChangeFlag type, SenderType senderType, QNodeId subjectId, Priority priority = Standard);
    virtual ~QSceneChange();

    ChangeFlag type() const Q_DECL_NOEXCEPT;
    qint64 timestamp() const Q_DECL_NOEXCEPT;
    QSceneChange::Priority priority() const Q_DECL_NOEXCEPT;
    QSceneChange::SenderType senderType() const Q_DECL_NOEXCEPT;
    QNodeId subjectId() const Q_DECL_NOEXCEPT;

protected:
    Q_DECLARE_PRIVATE(QSceneChange)
    QSceneChangePrivate *d_ptr;
    QSceneChange(QSceneChangePrivate &dd, ChangeFlag type, SenderType senderType, QNodeId subjectId, Priority priority = Standard);

private:
    Q_DISABLE_COPY(QSceneChange)
};

typedef QSharedPointer<QSceneChange> QSceneChangePtr;

} // namespace Qt3DCore

QT_END_NAMESPACE

#endif // QT3DCORE_QSCENECHANGE_H
