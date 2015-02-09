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

#include "texturedatamanager_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3D {

namespace Render {


void TextureDataManager::addToPendingTextures(const QNodeId &textureId)
{
    m_texturesPending.append(textureId);
}

QVector<QNodeId> TextureDataManager::texturesPending()
{
    QVector<QNodeId> textureIds = m_texturesPending;
    m_texturesPending.clear();
    return textureIds;
}

HTextureData TextureDataManager::textureDataFromFunctor(QTextureDataFunctorPtr functor) const
{
    QHash<QTextureDataFunctorPtr, HTextureData>::const_iterator it = m_textureDataFunctors.begin();
    const QHash<QTextureDataFunctorPtr, HTextureData>::const_iterator end = m_textureDataFunctors.end();
    while (it != end) {
        if (*it.key() == *functor)
            return it.value();
        ++it;
    }
    return HTextureData();
}

void TextureDataManager::addTextureDataForFunctor(HTextureData textureDataHandle, QTextureDataFunctorPtr functor)
{
    m_textureDataFunctors.insert(functor, textureDataHandle);
}

} // Render

} // Qt3D

QT_END_NAMESPACE
