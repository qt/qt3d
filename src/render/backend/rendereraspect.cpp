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

#include "rendereraspect.h"

#include "meshmanager.h"
#include "renderer.h"
#include "renderthread.h"

#include <loadmeshdatajob.h>
#include <updateworldtransformjob.h>
#include <updateboundingvolumejob.h>

#include <node.h>

#include <QDebug>
#include <QThread>
#include <QWindow>

#include <QtQml>

#include "scene.h"
#include "transform.h"
#include "effect.h"
#include "technique.h"
#include "tag.h"
#include "mesh.h"
#include "material.h"
#include "renderpass.h"
#include "shaderprogram.h"
#include "viewport.h"
#include "cameraselector.h"
#include "renderpassfilter.h"
#include "techniquefilter.h"
#include "rendertargetselector.h"
#include "shape.h"
#include "texture.h"
#include "framegraph.h"


namespace Qt3D {

RendererAspect::RendererAspect(QObject *parent)
    : AbstractAspect(AbstractAspect::AspectRenderer, parent)
    , m_renderThread(new RenderThread)
{
    qRegisterMetaType<QSurface*>("QSurface*");
    qRegisterMetaType< QSharedPointer<QObject> >("QObjectPtr");
    m_renderThread->waitForStart();
    m_renderThread->renderer()->setRendererAspect(this);
}

void RendererAspect::setWindow(QWindow *window)
{
    Q_ASSERT(window);
    QSurface* surface = window;

    Render::Renderer *renderer = m_renderThread->renderer();
    QMetaObject::invokeMethod(renderer, "setSurface", Q_ARG(QSurface*, surface));
}

QVector<QJobPtr> RendererAspect::jobsToExecute()
{
    // Create jobs that will get exectued by the threadpool
    QVector<QJobPtr> jobs;

//    Render::UpdateWorldTransformJobPtr worldTransformJob(new Render::UpdateWorldTransformJob(m_renderThread->renderer()->renderSceneRoot()));
//    Render::UpdateBoundingVolumeJobPtr boundingVolumeJob(new Render::UpdateBoundingVolumeJob(m_renderThread->renderer()));

//    // We can only update bounding volumes once all world transforms are known
//    boundingVolumeJob->addDependency(worldTransformJob);

//    // Add all jobs to queue
//    jobs.append(worldTransformJob);
//    jobs.append(boundingVolumeJob);

    // Create jobs to load in any meshes that are pending
    QList< QPair<QString,MeshDataPtr> > meshes = m_renderThread->renderer()->meshManager()->meshesPending();
    for (int i = 0; i < meshes.size(); ++i ) {
        const QPair<QString, MeshDataPtr> &meshDataInfo = meshes.at(i);
        Render::LoadMeshDataJobPtr loadMeshJob(new Render::LoadMeshDataJob(meshDataInfo.first, meshDataInfo.second));
        jobs.append(loadMeshJob);
    }
    m_renderThread->renderer()->meshManager()->clearMeshesPending();

    return jobs;
}

void RendererAspect::registerAspectHelper(Node *rootObject)
{
    Render::Renderer *renderer = m_renderThread->renderer();
    renderer->setSceneGraphRoot(rootObject);
}

void RendererAspect::unregisterAspectHelper(Node *rootObject)
{
    Q_UNUSED(rootObject);
}

void RendererAspect::initializeHelper(QAspectManager *aspectManager)
{
    Q_UNUSED(aspectManager);
    Render::Renderer *renderer = m_renderThread->renderer();
    QMetaObject::invokeMethod(renderer, "initialize");
}

void RendererAspect::cleanupHelper()
{
    //Render::Renderer *renderer = m_renderThread->renderer();
    //QMetaObject::invokeMethod(renderer, "cleanup");
}

/*!
 *  Registers the Qt3D Qml components defined by the aspect to the QmlEngine.
 */
void RendererAspect::registerQmlComponents()
{
    qmlRegisterType<Qt3D::Scene>("Qt3D.Render", 2, 0, "Scene");
    qmlRegisterType<Qt3D::Transform>("Qt3D.Render", 2, 0, "Transform");
    qmlRegisterType<Qt3D::Effect>("Qt3D.Render", 2, 0, "Effect");
    qmlRegisterType<Qt3D::Technique>("Qt3D.Render", 2, 0, "Technique");
    qmlRegisterType<Qt3D::Tag>("Qt3D.Render", 2, 0, "Tag");
    qmlRegisterType<Qt3D::Mesh>("Qt3D.Render", 2, 0, "Mesh");
    qmlRegisterType<Qt3D::Material>("Qt3D.Render", 2, 0, "Material");
    qmlRegisterType<Qt3D::RenderPass>("Qt3D.Render", 2, 0, "RenderPass");
    qmlRegisterType<Qt3D::ShaderProgram>("Qt3D.Render", 2, 0, "ShaderProgram");

    qmlRegisterType<Qt3D::Shape>("Qt3D.Render", 2, 0, "Shape");
    qmlRegisterType<Qt3D::QmlTexture>("Qt3D.Render", 2, 0, "Texture");

    // Framegraph components - TODO RenderTarget, RenderTargetSelector
    qmlRegisterType<Qt3D::Viewport>("Qt3D.Render", 2, 0, "Viewport");
    qmlRegisterType<Qt3D::CameraSelector>("Qt3D.Render", 2, 0, "CameraSelector");
    qmlRegisterType<Qt3D::RenderPassFilter>("Qt3D.Render", 2, 0, "RenderPassFilter");
    qmlRegisterType<Qt3D::TechniqueFilter>("Qt3D.Render", 2, 0, "TechniqueFilter");
    qmlRegisterType<Qt3D::Viewport>("Qt3D.Render", 2, 0, "Viewport");
    qmlRegisterType<Qt3D::FrameGraphItem>("Qt3D.Render", 2, 0, "FrameGraphNode");
    qmlRegisterType<Qt3D::RenderTargetSelector>("Qt3D.Render", 2, 0, "RenderTargetSelector");
    qmlRegisterType<Qt3D::FrameGraph>("Qt3D.Render", 2, 0, "FrameGraph");
}

}
