/****************************************************************************
**
** Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
******************************************************************************/

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
