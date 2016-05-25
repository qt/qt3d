/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
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

#include "texturedatamanager_p.h"
#include <Qt3DRender/qtextureimagedatagenerator.h>
QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {


TextureDataManager::TextureDataManager()
    : m_mutex(QMutex::Recursive)
{}

// Called from AspectThread sync
void TextureDataManager::addToPendingTextures(Qt3DCore::QNodeId textureId)
{
    // This simple check ensures that we won't be having n jobs
    // for one mesh, in case n Materials are referencing the said
    // texture
    if (!m_texturesPending.contains(textureId))
        m_texturesPending.append(textureId);
}

// Called from AspectThread prepare jobs
QVector<Qt3DCore::QNodeId> TextureDataManager::texturesPending()
{
    QVector<Qt3DCore::QNodeId> textureIds = m_texturesPending;
    m_texturesPending.clear();
    return textureIds;
}

// Called from LoadMeshDataJob threads
HTextureData TextureDataManager::textureDataFromFunctor(const QTextureImageDataGeneratorPtr &functor) const
{
    QMutexLocker lock(&m_mutex);
    for (int i = 0, m = m_textureDataFunctors.size(); i < m; ++i) {
        if (*functor == *(m_textureDataFunctors[i].first))
            return m_textureDataFunctors[i].second;
    }
    return HTextureData();
}

// Called from LoadMeshDataJob threads
void TextureDataManager::addTextureDataForFunctor(HTextureData textureDataHandle, const QTextureImageDataGeneratorPtr &functor)
{
    QMutexLocker lock(&m_mutex);
    m_textureDataFunctors.push_back(qMakePair(functor, textureDataHandle));
}

// Called from LoadMeshDataJob threads
void TextureDataManager::removeTextureDataFunctor(const QTextureImageDataGeneratorPtr &functor)
{
    QMutexLocker lock(&m_mutex);
    for (int i = 0, m = m_textureDataFunctors.size(); i < m; ++i) {
        if (*functor == *(m_textureDataFunctors[i].first)) {
            m_textureDataFunctors.remove(i);
            break;
        }
    }
}

// Called from LoadMeshDataJob threads
void TextureDataManager::assignFunctorToTextureImage(const QTextureImageDataGeneratorPtr &newFunctor, HTextureImage imageHandle)
{
    QMutexLocker lock(&m_mutex);
    QVector<QPair<QTextureImageDataGeneratorPtr, QVector<HTextureImage> > >::iterator it = m_texturesImagesPerFunctor.begin();

    bool newFunctorAlreadyExists = false;
    bool oldFunctorWasRemoved = false;
    while (it != m_texturesImagesPerFunctor.end()) {
        QPair<QTextureImageDataGeneratorPtr, QVector<HTextureImage> > &entry = *it;
        QTextureImageDataGeneratorPtr functor = entry.first;
        QVector<HTextureImage> &imageHandles = entry.second;
        const bool functorMatchesNewFunctor = (*functor == *newFunctor);
        bool removed = false;

        if (functorMatchesNewFunctor) {
            // New Functor already exist, just need to append
            imageHandles.push_back(imageHandle);
            newFunctorAlreadyExists = true;
        } else if (imageHandles.contains(imageHandle)) {
            // functor in array != New Functor and contains
            // <=> functor was the previous functor of imageHandle
            // the image handle, we need to remove the handle
            imageHandles.removeAll(imageHandle);

            // If no texture image is referencing the functor anymore
            // get rid of the functor
            if (imageHandles.isEmpty()) {
                // We need to release the texture image data
                HTextureData textureDataHandle = textureDataFromFunctor(functor);
                if (!m_textureHandlesToRelease.contains(textureDataHandle))
                    m_textureHandlesToRelease.push_back(textureDataHandle);
                // Remove functor
                removeTextureDataFunctor(functor);
                // Remove the entry
                it = m_texturesImagesPerFunctor.erase(it);
                removed = true;
                oldFunctorWasRemoved = true;
            }
        }

        // Early exit if we can
        if (oldFunctorWasRemoved && newFunctorAlreadyExists)
            break;

        if (!removed)
            ++it;
    }

    // Insert new imageHandle with new functor
    if (!newFunctorAlreadyExists)
        m_texturesImagesPerFunctor.push_back(qMakePair(newFunctor, QVector<HTextureImage>() << imageHandle));
}

QMutex *TextureDataManager::mutex() const
{
    return &m_mutex;
}

// Called by cleanup job
// No need to lock
void TextureDataManager::cleanup()
{
   for (int i = 0, m = m_textureHandlesToRelease.size(); i < m; ++i)
       release(m_textureHandlesToRelease[i]);
   m_textureHandlesToRelease.clear();
}

} // namespace Render

} // namespace Qt3DRender

QT_END_NAMESPACE
