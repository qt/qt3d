// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DCORE_QBACKENDNODE_P_H
#define QT3DCORE_QBACKENDNODE_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <Qt3DCore/qbackendnode.h>
#include <Qt3DCore/qnodeid.h>
#include <Qt3DCore/private/qt3dcore_global_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

class QNode;

class Q_3DCORE_PRIVATE_EXPORT QBackendNodePrivate
{
public:
    QBackendNodePrivate(QBackendNode::Mode mode);
    virtual ~QBackendNodePrivate();

    void setEnabled(bool enabled);

    static QBackendNodePrivate *get(QBackendNode *n);

    Q_DECLARE_PUBLIC(QBackendNode)
    QBackendNode *q_ptr;
    QBackendNode::Mode m_mode;

    QNodeId m_peerId;
    bool m_enabled;

    virtual void addedToEntity(QNode *frontend);
    virtual void removedFromEntity(QNode *frontend);
    virtual void componentAdded(QNode *frontend);
    virtual void componentRemoved(QNode *frontend);

private:
    Q_DISABLE_COPY(QBackendNodePrivate)
};

} // Qt3D

QT_END_NAMESPACE

#endif // QT3DCORE_QBACKENDNODE_P_H
