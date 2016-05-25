/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "loadtexturedatajob_p.h"
#include <Qt3DRender/private/nodemanagers_p.h>
#include <Qt3DRender/private/managers_p.h>
#include <Qt3DRender/private/texturedatamanager_p.h>
#include <Qt3DRender/private/qtextureimage_p.h>
#include <Qt3DRender/qtextureimagedata.h>
#include <Qt3DRender/qtexturedata.h>
#include <QThread>
#include <Qt3DRender/private/job_common_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {

namespace {

typedef QPair<HTextureData, QTextureImageData *> HandleDataPair;

HandleDataPair textureDataFromGenerator(TextureDataManager *textureDataManager,
                                        QTextureImageDataGeneratorPtr generator)
{
    HTextureData textureDataHandle;
    QTextureImageData *data = nullptr;

    QMutexLocker locker(textureDataManager->mutex());
    // We don't want to take the chance of having two jobs uploading the same functor
    // because of sync issues

    textureDataHandle = textureDataManager->textureDataFromFunctor(generator);

    // Texture data handle isn't null == there's already a matching TextureData
    if (!textureDataHandle.isNull()) {
        data = textureDataManager->data(textureDataHandle);
    } else {
        // Texture data is null -> we need to generate it
        QTextureImageDataPtr dataPtr = generator->operator ()();
        if (dataPtr.isNull()) {
            qCDebug(Jobs) << Q_FUNC_INFO << "Texture has no raw data";
        } else {
            // Save the QTextureImageDataPtr with it's functor as a key
            textureDataHandle = textureDataManager->acquire();
            data = textureDataManager->data(textureDataHandle);
            *data = *(dataPtr.data());
            textureDataManager->addTextureDataForFunctor(textureDataHandle, generator);
        }
    }

    return qMakePair(textureDataHandle, data);
}

void createTextureFromGenerator(TextureDataManager *textureDataManager,
                                Texture *texture)
{
    QTextureGeneratorPtr generator = texture->dataGenerator();
    const QTextureDataPtr generatedData = generator->operator ()();

    // TO DO set the status of the texture based on the status of the functor

    // Use the first QTexImageData loaded to determine the target / mipmaps
    // if not specified

    if (texture->target() != QAbstractTexture::TargetAutomatic)
        qWarning() << "When a texture provides a generator, it's target is expected to be TargetAutomatic";

    texture->setTarget(static_cast<QAbstractTexture::Target>(generatedData->target()));
    texture->setSize(generatedData->width(), generatedData->height(), generatedData->depth());
    texture->setLayers(generatedData->layers());
    texture->setFormat(generatedData->format());

    // Note: These texture data handles aren't associated with a QTextureImageDataGenerator
    // and will therefore be destroyed when the Texture element is destroyed or cleaned up
    const QVector<QTextureImageDataPtr> imageData = generatedData->imageData();

    if (imageData.size() > 0) {
        QMutexLocker locker(textureDataManager->mutex());
        // We don't want to take the chance of having two jobs uploading the same functor
        // because of sync issues

        // Set the mips level based on the first image if autoMipMapGeneration is disabled
        if (!texture->isAutoMipMapGenerationEnabled())
            texture->setMipLevels(imageData.first()->mipLevels());

        for (QTextureImageDataPtr dataPtr : imageData) {
            HTextureData textureDataHandle = textureDataManager->acquire();
            QTextureImageData *data = textureDataManager->data(textureDataHandle);
            *data = *(dataPtr.data());
            texture->addTextureDataHandle(textureDataHandle);
        }
    }
}

} // anonymous

LoadTextureDataJob::LoadTextureDataJob(Qt3DCore::QNodeId textureId)
    : m_textureId(textureId)
{
    SET_JOB_RUN_STAT_TYPE(this, JobTypes::LoadTextureData, 0);
}

LoadTextureDataJob::~LoadTextureDataJob()
{
}

void LoadTextureDataJob::run()
{
    qCDebug(Jobs) << "Entering" << Q_FUNC_INFO << QThread::currentThread();

    Texture *txt = m_manager->textureManager()->lookupResource(m_textureId);
    TextureDataManager *textureDataManager = m_manager->manager<QTextureImageData, TextureDataManager>();

    if (txt != nullptr) {
        // We need to clear the TextureData handles of the texture in case it was previously
        // loaded with a different functor
        txt->releaseTextureDataHandles();

        // If the texture has a functor we used it to generate embedded TextureImages
        if (txt->dataGenerator())
            createTextureFromGenerator(textureDataManager, txt);

        // Load update each TextureImage
        const auto texImgHandles = txt->textureImages();
        for (HTextureImage texImgHandle : texImgHandles) {
            TextureImage *texImg = m_manager->textureImageManager()->data(texImgHandle);
            if (texImg != nullptr && texImg->isDirty() && !texImg->dataGenerator().isNull()) {
                QTextureImageDataGeneratorPtr generator = texImg->dataGenerator();

                QPair<HTextureData, QTextureImageData *> handleData = textureDataFromGenerator(textureDataManager, generator);

                // If using QTextureImage, notify the frontend of the change in status
                const QImageTextureDataFunctor *imageGenerator = functor_cast<QImageTextureDataFunctor>(generator.data());
                if (imageGenerator)
                    texImg->setStatus(imageGenerator->status());

                HTextureData textureDataHandle = handleData.first;
                QTextureImageData *data = handleData.second;

                // XXX released textureDataManager mutex, do we have a race here?

                // Update HTextureImage Functor to release TextureData when needed
                TextureDataManager *textureDataManager = m_manager->manager<QTextureImageData, TextureDataManager>();
                textureDataManager->assignFunctorToTextureImage(generator, texImgHandle);

                // Set texture size of texture if the first layer / level / face has a valid size
                // otherwise assume the size was set on the texture itself
                if (texImg->layer() == 0 && texImg->mipLevel() == 0 &&
                        texImg->face() == QAbstractTexture::CubeMapPositiveX) {

                    if (data == nullptr) {
                        qWarning() << "Texture data is null, texture data failed to load";
                    } else {
                        // Set the size of the texture based on the layer 0 / level 0
                        // if the functor provides something valid. Otherwise we assume the texture
                        // already has the correct size
                        if (data->width() != -1 && data->height() != -1 && data->depth() != -1) {
                            txt->setSize(data->width(), data->height(), data->depth());
                        }
                        // Set the format of the texture if the texture format is set to Automatic
                        if (txt->format() == QAbstractTexture::Automatic) {
                            txt->setFormat(static_cast<QAbstractTexture::TextureFormat>(data->format()));
                        }
                    }
                }
                // Set the textureDataHandle on the texture image
                // Note: this internally updates the DNA of the TextureImage
                texImg->setTextureDataHandle(textureDataHandle);
            }
        }
        // Tell the renderer to reload/upload to GPU the TextureImage for the Texture
        // next frame
        txt->requestTextureDataUpdate();
    }
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
