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

#include "quick3dtexture.h"
#include <QDebug>

QT_BEGIN_NAMESPACE

namespace Qt3D {

namespace Render {

namespace Quick {

Quick3DTextureExtension::Quick3DTextureExtension(QObject *parent)
    : QObject(parent)
{
}

QQmlListProperty<QAbstractTextureImage> Quick3DTextureExtension::textureImages()
{
    return QQmlListProperty<QAbstractTextureImage>(this, 0,
                                                   &Quick3DTextureExtension::appendTextureImage,
                                                   &Quick3DTextureExtension::textureImageCount,
                                                   &Quick3DTextureExtension::textureImageAt,
                                                   &Quick3DTextureExtension::clearTextureImageList);
}

void Quick3DTextureExtension::appendTextureImage(QQmlListProperty<QAbstractTextureImage> *list, QAbstractTextureImage *textureImage)
{
    Quick3DTextureExtension *self = qobject_cast<Quick3DTextureExtension *>(list->object);
    if (self)
        self->parentTexture()->addTextureImage(textureImage);
}

QAbstractTextureImage *Quick3DTextureExtension::textureImageAt(QQmlListProperty<QAbstractTextureImage> *list, int index)
{
    Quick3DTextureExtension *self = qobject_cast<Quick3DTextureExtension *>(list->object);
    if (self)
        return self->parentTexture()->textureImages().at(index);
    return Q_NULLPTR;
}

int Quick3DTextureExtension::textureImageCount(QQmlListProperty<QAbstractTextureImage> *list)
{
    Quick3DTextureExtension *self = qobject_cast<Quick3DTextureExtension *>(list->object);
    if (self)
        return self->parentTexture()->textureImages().count();
    return 0;
}

void Quick3DTextureExtension::clearTextureImageList(QQmlListProperty<QAbstractTextureImage> *list)
{
    Quick3DTextureExtension *self = qobject_cast<Quick3DTextureExtension *>(list->object);
    if (self)
        Q_FOREACH (QAbstractTextureImage *img, self->parentTexture()->textureImages())
            self->parentTexture()->removeTextureImage(img);
}

} // Quick

} // Render

} // Qt3D

QT_END_NAMESPACE
