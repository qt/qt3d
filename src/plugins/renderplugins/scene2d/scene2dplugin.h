// Copyright (C) 2017 The Qt Company Ltd and/or its subsidiary(-ies).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_SCENE2DPLUGIN_H
#define QT3DRENDER_SCENE2DPLUGIN_H

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

#include <private/qrenderaspect_p.h>
#include <private/qrenderplugin_p.h>
#include <private/abstractrenderer_p.h>
#include <private/qresourcemanager_p.h>
#include <private/scene2d_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {

class Scene2DNode;

class Scene2DNodeManager : public Qt3DCore::QResourceManager<
        Render::Quick::Scene2D,
        Qt3DCore::QNodeId,
        Qt3DCore::ObjectLevelLockingPolicy>
{
};

class Scene2DPlugin : public QRenderPlugin
{
public:
    Scene2DPlugin();
    ~Scene2DPlugin();
    bool registerBackendTypes(QRenderAspect *aspect, AbstractRenderer *renderer) override;
    bool unregisterBackendTypes(QRenderAspect *aspect) override;
private:
    Render::Scene2DNodeManager *m_scene2dNodeManager;
};

} // Render
} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_SCENE2DPLUGIN_H
