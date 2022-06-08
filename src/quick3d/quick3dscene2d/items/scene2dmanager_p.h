// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QUICK3DRENDER_SCENE2DMANAGER_P_H
#define QT3DRENDER_QUICK3DRENDER_SCENE2DMANAGER_P_H

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

#include <Qt3DQuickScene2D/qt3dquickscene2d_global.h>
#include <Qt3DQuickScene2D/qscene2d.h>

#include <QtQml/QQmlEngine>
#include <QtQml/QQmlComponent>
#include <QtQuick/QQuickItem>

#include <private/qnode_p.h>

#include <QtCore/qobject.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Quick {

class QScene2DPrivate;
class Scene2DSharedObject;

class Scene2DManager : public QObject
{
    Q_OBJECT
public:
    Scene2DManager(QScene2DPrivate *priv);
    ~Scene2DManager();

    QQuickItem *m_rootItem;
    QQuickItem *m_item;

    QScene2DPrivate *m_priv;
    QSharedPointer<Scene2DSharedObject> m_sharedObject;

    Qt3DCore::QNodeId m_id;
    QScene2D::RenderPolicy m_renderPolicy;

    bool m_requested;
    bool m_initialized;
    bool m_renderSyncRequested;
    bool m_backendInitialized;
    bool m_mouseEnabled;

    void requestRender();
    void requestRenderSync();
    void doRenderSync();
    void startIfInitialized();
    void stopAndClean();
    void updateSizes();

    void setItem(QQuickItem *item);

    bool event(QEvent *e) override;

    void cleanup();
};

} // namespace Quick
} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QUICK3DRENDER_SCENE2DMANAGER_P_H
