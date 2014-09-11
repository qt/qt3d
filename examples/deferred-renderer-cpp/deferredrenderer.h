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

#ifndef DEFERREDRENDERER_H
#define DEFERREDRENDERER_H

#include <Qt3DRenderer/QViewport>
#include <Qt3DRenderer/QClearBuffer>
#include <Qt3DRenderer/QLayerFilter>
#include <Qt3DRenderer/QRenderPassFilter>
#include <Qt3DRenderer/QRenderTargetSelector>
#include <Qt3DRenderer/QCameraSelector>
#include <Qt3DRenderer/QCriterion>

class DeferredRenderer : public Qt3D::QViewport
{
public:
    explicit DeferredRenderer(Qt3D::QNode *parent = 0);

    void setSceneCamera(Qt3D::QEntity *camera);
    void setGBuffer(Qt3D::QRenderTarget *gBuffer);
    void setGeometryPassCriteria(QList<Qt3D::QCriterion *> criteria);
    void setFinalPassCriteria(QList<Qt3D::QCriterion *> criteria);
    void setGBufferLayer(const QString &layerName);
    void setScreenQuadLayer(const QString &layerName);

private:
    Qt3D::QClearBuffer *m_clearGBuffer;
    Qt3D::QClearBuffer *m_clearScreenQuad;
    Qt3D::QLayerFilter *m_sceneFilter;
    Qt3D::QLayerFilter *m_screenQuadFilter;
    Qt3D::QRenderTargetSelector *m_gBufferTargetSelector;
    Qt3D::QRenderPassFilter *m_geometryPassFilter;
    Qt3D::QRenderPassFilter *m_finalPassFilter;
    Qt3D::QCameraSelector *m_sceneCameraSelector;
};

#endif // DEFERREDRENDERER_H
