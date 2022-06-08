// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QUICK3DSCENE2D_QSCENE2D_P_H
#define QT3DRENDER_QUICK3DSCENE2D_QSCENE2D_P_H

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

#include <Qt3DQuickScene2D/qscene2d.h>

#include <private/qnode_p.h>
#include <private/scene2dsharedobject_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {
class QScene;
}

namespace Qt3DRender {

namespace Quick {

class QScene2D;
class Scene2DManager;

class Q_AUTOTEST_EXPORT QScene2DPrivate : public Qt3DCore::QNodePrivate
{
public:
    Q_DECLARE_PUBLIC(QScene2D)

    QScene2DPrivate();
    ~QScene2DPrivate();

    Scene2DManager *m_renderManager;
    QMetaObject::Connection m_textureDestroyedConnection;
    Qt3DRender::QRenderTargetOutput *m_output;
    QList<Qt3DCore::QEntity *> m_entities;
};

struct QScene2DData
{
    QScene2D::RenderPolicy renderPolicy;
    Scene2DSharedObjectPtr sharedObject;
    Qt3DCore::QNodeId output;
    QList<Qt3DCore::QNodeId> entityIds;
    bool mouseEnabled;
};

} // namespace Quick
} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QUICK3DSCENE2D_QSCENE2D_P_H
