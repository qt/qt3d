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

#include "forwardrenderer.h"

ForwardRenderer::ForwardRenderer(Qt3D::QNode *parent)
    : Qt3D::QFrameGraph(parent)
    , m_viewport(new Qt3D::QViewport())
    , m_cameraSelector(new Qt3D::QCameraSelector())
    , m_clearBuffer(new Qt3D::QClearBuffer())
{
    m_viewport->setRect(QRectF(0.0f, 0.0f, 1.0f, 1.0f));
    m_viewport->setClearColor(Qt::white);
    m_clearBuffer->setBuffers(Qt3D::QClearBuffer::ColorDepthBuffer);
    m_cameraSelector->setParent(m_viewport);
    m_clearBuffer->setParent(m_cameraSelector);
    setActiveFrameGraph(m_viewport);
}

void ForwardRenderer::setCamera(Qt3D::QEntity *camera)
{
    m_cameraSelector->setCamera(camera);
}
