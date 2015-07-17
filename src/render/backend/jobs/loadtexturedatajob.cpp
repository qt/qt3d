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

#include "loadtexturedatajob_p.h"
#include <Qt3DRenderer/private/renderer_p.h>
#include <Qt3DRenderer/private/managers_p.h>
#include <Qt3DRenderer/private/texturedatamanager_p.h>
#include <QThread>

QT_BEGIN_NAMESPACE

namespace Qt3D {

namespace Render {

LoadTextureDataJob::LoadTextureDataJob(const QNodeId &textureId)
    : m_textureId(textureId)
{
}

LoadTextureDataJob::~LoadTextureDataJob()
{
}

void LoadTextureDataJob::run()
{
    qCDebug(Jobs) << "Entering" << Q_FUNC_INFO << QThread::currentThread();

    RenderTexture *txt = m_renderer->textureManager()->lookupResource(m_textureId);

    if (txt != Q_NULLPTR) {
        // Load update each TextureImage
        Q_FOREACH (HTextureImage texImgHandle, txt->textureImages()) {
            RenderTextureImage *texImg = m_renderer->textureImageManager()->data(texImgHandle);
            if (texImg != Q_NULLPTR && texImg->isDirty() && !texImg->dataFunctor().isNull()) {
                QTextureDataFunctorPtr functor = texImg->dataFunctor();
                HTextureData textureDataHandle;
                TexImageData *data = Q_NULLPTR;

                // scoped for locker
                {
                    QMutexLocker locker(m_renderer->textureDataManager()->mutex());
                    // We don't want to take the chance of having two jobs uploading the same functor
                    // because of sync issues
                    textureDataHandle = m_renderer->textureDataManager()->textureDataFromFunctor(functor);

                    // Texture data handle isn't null == there's already a matching TextureData
                    if (!textureDataHandle.isNull()) {
                        data = m_renderer->textureDataManager()->data(textureDataHandle);
                    } else {
                        TexImageDataPtr dataPtr = functor->operator ()();
                        if (dataPtr.isNull()) {
                            qCDebug(Jobs) << Q_FUNC_INFO << "Texture has no raw data";
                        } else {
                            // Save the TexImageDataPtr with it's functor as a key
                            textureDataHandle = m_renderer->textureDataManager()->acquire();
                            data = m_renderer->textureDataManager()->data(textureDataHandle);
                            *data = *(dataPtr.data());
                            m_renderer->textureDataManager()->addTextureDataForFunctor(textureDataHandle, functor);
                        }
                    }

                    // Update HTextureImage Functor to release TextureData when needed
                    m_renderer->textureDataManager()->assignFunctorToTextureImage(functor, texImgHandle);
                }

                // Set texture size of texture if the first layer / level / face has a valid size
                // otherwise assume the size was set on the texture itself
                if (texImg->layer() == 0 && texImg->mipmapLevel() == 0 &&
                        texImg->face() == QAbstractTextureProvider::CubeMapPositiveX) {

                    if (data == Q_NULLPTR) {
                        qWarning() << "Texture data is null, texture data failed to load";
                    } else {
                        // Set the size of the texture based on the layer 0 / level 0
                        // if the functor provides something valid. Otherwise we assume the texture
                        // already has the correct size
                        if (data->width() != -1 && data->height() != -1 && data->depth() != -1) {
                            txt->setSize(data->width(), data->height(), data->depth());
                        }
                        // Set the format of the texture if the texture format is set to Automatic
                        if (txt->format() == QAbstractTextureProvider::Automatic) {
                            txt->setFormat(static_cast<QAbstractTextureProvider::TextureFormat>(data->format()));
                        }
                    }
                }
                // Set the textureDataHandle on the texture image
                texImg->setTextureDataHandle(textureDataHandle);
            }
        }
        // Tell the renderer to reload the TextureImage for the Texture
        // next frame
        txt->requestTextureDataUpdate();
    }
}

} // Render

} // Qt3D

QT_END_NAMESPACE
