/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
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

#ifndef DEFERREDRENDERER_H
#define DEFERREDRENDERER_H

#include <Qt3DRender/QViewport>
#include <Qt3DRender/QClearBuffer>
#include <Qt3DRender/QLayerFilter>
#include <Qt3DRender/QRenderPassFilter>
#include <Qt3DRender/QRenderTargetSelector>
#include <Qt3DRender/QCameraSelector>
#include <Qt3DRender/QAnnotation>

class DeferredRenderer : public Qt3DRender::QViewport
{
public:
    explicit DeferredRenderer(Qt3DCore::QNode *parent = 0);

    void setSceneCamera(Qt3DCore::QEntity *camera);
    void setGBuffer(Qt3DRender::QRenderTarget *gBuffer);
    void setGeometryPassCriteria(QList<Qt3DRender::QAnnotation *> criteria);
    void setFinalPassCriteria(QList<Qt3DRender::QAnnotation *> criteria);
    void setGBufferLayers(const QStringList &layerName);
    void setScreenQuadLayers(const QStringList &layerName);

private:
    Qt3DRender::QLayerFilter *m_sceneFilter;
    Qt3DRender::QLayerFilter *m_screenQuadFilter;
    Qt3DRender::QClearBuffer *m_clearScreenQuad;
    Qt3DRender::QRenderTargetSelector *m_gBufferTargetSelector;
    Qt3DRender::QClearBuffer *m_clearGBuffer;
    Qt3DRender::QRenderPassFilter *m_geometryPassFilter;
    Qt3DRender::QRenderPassFilter *m_finalPassFilter;
    Qt3DRender::QCameraSelector *m_sceneCameraSelector;
};

#endif // DEFERREDRENDERER_H
