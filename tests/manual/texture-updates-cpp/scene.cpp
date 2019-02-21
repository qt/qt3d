/****************************************************************************
**
** Copyright (C) 2019 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "scene.h"

#include <QtCore/QDebug>
#include <QtCore/QTimer>
#include <QtCore/QDateTime>

#include <Qt3DExtras/QCuboidMesh>
#include <Qt3DExtras/QDiffuseMapMaterial>
#include <Qt3DRender/QTextureDataUpdate>
#include <cstring>


Scene::Scene(Qt3DCore::QEntity *rootEntity)
    : m_rootEntity(rootEntity)
{
    // Cuboid shape data
    Qt3DExtras::QCuboidMesh *cuboid = new Qt3DExtras::QCuboidMesh();

    // CuboidMesh Transform
    m_transform = new Qt3DCore::QTransform();
    m_transform->setScale(4.0f);;

    // create texture
    m_texture = new Qt3DRender::QTexture2D();
    m_texture->setWidth(512);
    m_texture->setHeight(512);
    m_texture->setFormat(Qt3DRender::QTexture2D::RGBA8_UNorm);
    m_texture->setLayers(1);
    m_texture->setGenerateMipMaps(false);
    m_texture->setMinificationFilter(Qt3DRender::QTexture2D::Nearest);
    m_texture->setMagnificationFilter(Qt3DRender::QTexture2D::Nearest);

    Qt3DExtras::QDiffuseMapMaterial *mat = new Qt3DExtras::QDiffuseMapMaterial();
    mat->setAmbient(QColor(64, 64, 64));
    mat->setSpecular(QColor(255, 255, 255));
    mat->setDiffuse(m_texture);

    //Cuboid
    m_cuboidEntity = new Qt3DCore::QEntity(m_rootEntity);
    m_cuboidEntity->addComponent(cuboid);
    m_cuboidEntity->addComponent(mat);
    m_cuboidEntity->addComponent(m_transform);
}

Scene::~Scene()
{
}

void Scene::run()
{
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Scene::updateTimer);
    timer->start(50);
}

void Scene::updateTimer()
{
    static int yPos = 0;
    static uchar pixelValue = 255;

    m_transform->setRotation(QQuaternion::fromAxisAndAngle(0, 1, 0, m_angle));
    m_angle += 0.4f;

    QByteArray rawPixelData;
    const int rows = 32;
    rawPixelData.resize(512 * 4 * rows * sizeof(uchar));
    uchar *pixels = reinterpret_cast<uchar *>(rawPixelData.data());
    std::memset(pixels, pixelValue, 512 * rows * 4 * sizeof(uchar));

    // QTextureImageData hold our raw content and size of subcontent
    Qt3DRender::QTextureImageDataPtr imageData = Qt3DRender::QTextureImageDataPtr::create();
    imageData->setWidth(512);
    imageData->setHeight(rows);
    imageData->setMipLevels(1);
    imageData->setLayers(1);
    // We upload the 4 components of an RGBA pixel as 4 uchar
    imageData->setPixelFormat(QOpenGLTexture::RGBA);
    imageData->setPixelType(QOpenGLTexture::UInt8);
    imageData->setFormat(QOpenGLTexture::RGBA8_UNorm);
    imageData->setData(rawPixelData, 4, false);

    // QTextureDataUpdate references our updated content
    // as well as x, y offsets where content needs to be uploaded
    Qt3DRender::QTextureDataUpdate update;
    update.setX(0);
    update.setY(yPos);
    update.setMipLevel(0);
    update.setData(imageData);
    // Send update to texture
    m_texture->updateData(update);

    if (yPos + rows >= 512)
        pixelValue = pixelValue > 0U ? 0U : 255U;
    yPos = (yPos + rows) % 512;

}
