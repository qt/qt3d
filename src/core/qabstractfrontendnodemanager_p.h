// Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DCORE_QABSTRACTFRONTENDNODEMANAGER_P_H
#define QT3DCORE_QABSTRACTFRONTENDNODEMANAGER_P_H

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

#include <Qt3DCore/qt3dcore_global.h>
#include <Qt3DCore/qnodeid.h>
#include <private/qglobal_p.h>

#include <QtCore/qlist.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

class QNode;

class Q_3DCORESHARED_EXPORT QAbstractFrontEndNodeManager
{
public:
    // Changes made to backend nodes are reported to the Renderer
    enum DirtyNodeFlag {
        TransformDirty      = 1 << 0,
        GeometryDirty       = 1 << 1,
        EntityEnabledDirty  = 1 << 2,
        BuffersDirty        = 1 << 3,
        AllDirty            = 0xffffff
    };
    Q_DECLARE_FLAGS(DirtyNodeSet, DirtyNodeFlag)

    virtual ~QAbstractFrontEndNodeManager();

    virtual QNode *lookupNode(QNodeId id) const = 0;
    virtual QList<QNode *> lookupNodes(const QList<QNodeId> &ids) const = 0;

protected:
    QAbstractFrontEndNodeManager();
};

} // Qt3DCore

QT_END_NAMESPACE

#endif // QT3DCORE_QABSTRACTFRONTENDNODEMANAGER_P_H
