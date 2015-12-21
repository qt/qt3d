/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/
#ifndef QT3DRENDER_RENDER_ABSTRACTRENDERER_P_H
#define QT3DRENDER_RENDER_ABSTRACTRENDERER_P_H

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

#include <Qt3DRender/private/qt3drender_global_p.h>
#include <Qt3DCore/qaspectjob.h>
#include <Qt3DCore/qnodeid.h>

QT_BEGIN_NAMESPACE

class QSurface;

namespace Qt3DCore {
class QAbstractFrameAdvanceService;
class QBackendNodeFactory;
class QEventFilterService;
class QAbstractAspectJobManager;
class QServiceLocator;
}

namespace Qt3DRender {

class QRenderAspect;

namespace Render {

class NodeManagers;
class Entity;
class FrameGraphNode;

class QT3DRENDERSHARED_PRIVATE_EXPORT AbstractRenderer
{
public:
    virtual ~AbstractRenderer() {}

    enum API {
        OpenGL
    };

    virtual API api() const = 0;

    virtual qint64 time() const = 0;
    virtual void setTime(qint64 time) = 0;

    virtual void setSurface(QSurface *surface) = 0;
    virtual void setNodeManagers(NodeManagers *managers) = 0;
    virtual void setServices(Qt3DCore::QServiceLocator *services) = 0;
    virtual void setSurfaceExposed(bool exposed) = 0;

    virtual QSurface *surface() const = 0;
    virtual NodeManagers *nodeManagers() const = 0;
    virtual Qt3DCore::QServiceLocator *services() const = 0;

    virtual void initialize() = 0;
    virtual void shutdown() = 0;
    virtual void createAllocators(Qt3DCore::QAbstractAspectJobManager *jobManager) = 0;
    virtual void destroyAllocators(Qt3DCore::QAbstractAspectJobManager *jobManager) = 0;

    // Threaded renderer
    virtual void render() = 0;
    // Synchronous renderer
    virtual void doRender() = 0;

    virtual bool isRunning() const = 0;

    virtual QVector<Qt3DCore::QAspectJobPtr> renderBinJobs() = 0;
    virtual Qt3DCore::QAspectJobPtr pickBoundingVolumeJob() = 0;

    virtual void setFrameGraphRoot(const Qt3DCore::QNodeId fgRootId) = 0;
    virtual void setSceneRoot(Qt3DCore::QBackendNodeFactory *factory, Entity *root) = 0;

    virtual Entity *sceneRoot() const = 0;
    virtual FrameGraphNode *frameGraphRoot() const = 0;

    virtual Qt3DCore::QAbstractFrameAdvanceService *frameAdvanceService() const = 0;
    virtual void registerEventFilter(Qt3DCore::QEventFilterService *service) = 0;

};

} // Render

} // Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_ABSTRACTRENDERER_P_H

