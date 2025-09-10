// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// Copyright (C) 2016 The Qt Company Ltd and/or its subsidiary(-ies).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_ENTITY_P_P_H
#define QT3DRENDER_RENDER_ENTITY_P_P_H

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

#include <Qt3DRender/private/entity_p.h>
#include <Qt3DCore/private/qbackendnode_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {
class QNode;
}

namespace Qt3DRender {

class QRenderAspect;

namespace Render {

class Q_AUTOTEST_EXPORT EntityPrivate : public Qt3DCore::QBackendNodePrivate {
public:
    EntityPrivate();

    Q_DECLARE_PUBLIC(Entity)

    static EntityPrivate *get(Entity *node);

    void componentAdded(Qt3DCore::QNode *frontend) override;
    void componentRemoved(Qt3DCore::QNode *frontend) override;
};

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_ENTITY_P_P_H
