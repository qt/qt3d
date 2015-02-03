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

#include "quick3dtexture.h"
#include <QDebug>

QT_BEGIN_NAMESPACE

namespace Qt3D {

namespace Render {

namespace Quick {

Quick3DTextureCubeMapExtension::Quick3DTextureCubeMapExtension(QObject *parent)
    : QObject(parent)
{
}

void Quick3DTextureCubeMapExtension::setSource(const QUrl &source)
{
    if (source != m_source) {
        m_source = source;
        emit sourceChanged();
        QUrl tmp(m_source);
        // Default to png extension for now
        tmp.setPath(m_source.path() + QStringLiteral("_posx.png"));
        setPositiveX(tmp);
        tmp.setPath(m_source.path() + QStringLiteral("_posy.png"));
        setPositiveY(tmp);
        tmp.setPath(m_source.path() + QStringLiteral("_posz.png"));
        setPositiveZ(tmp);
        tmp.setPath(m_source.path() + QStringLiteral("_negx.png"));
        setNegativeX(tmp);
        tmp.setPath(m_source.path() + QStringLiteral("_negy.png"));
        setNegativeY(tmp);
        tmp.setPath(m_source.path() + QStringLiteral("_negz.png"));
        setNegativeZ(tmp);
    }
}

void Quick3DTextureCubeMapExtension::setPositiveX(const QUrl &positiveX)
{
    if (positiveX != m_positiveX) {
        m_positiveX = positiveX;
        emit positiveXChanged();
        loadFace(positiveX, QAbstractTextureProvider::CubeMapPositiveX);
    }
}

void Quick3DTextureCubeMapExtension::setPositiveY(const QUrl &positiveY)
{
    if (positiveY != m_positiveY) {
        m_positiveY = positiveY;
        emit positiveYChanged();
        loadFace(positiveY, QAbstractTextureProvider::CubeMapPositiveY);
    }
}

void Quick3DTextureCubeMapExtension::setPositiveZ(const QUrl &positiveZ)
{
    if (positiveZ != m_positiveZ) {
        m_positiveZ = positiveZ;
        emit positiveZChanged();
        loadFace(positiveZ, QAbstractTextureProvider::CubeMapPositiveZ);
    }
}

void Quick3DTextureCubeMapExtension::setNegativeX(const QUrl &negativeX)
{
    if (negativeX != m_negativeX) {
        m_negativeX = negativeX;
        emit negativeXChanged();
        loadFace(negativeX, QAbstractTextureProvider::CubeMapNegativeX);
    }
}

void Quick3DTextureCubeMapExtension::setNegativeY(const QUrl &negativeY)
{
    if (negativeY != m_negativeY) {
        m_negativeY = negativeY;
        emit negativeYChanged();
        loadFace(negativeY, QAbstractTextureProvider::CubeMapNegativeY);
    }
}

void Quick3DTextureCubeMapExtension::setNegativeZ(const QUrl &negativeZ)
{
    if (negativeZ != m_negativeZ) {
        m_negativeZ = negativeZ;
        emit negativeZChanged();
        loadFace(negativeZ, QAbstractTextureProvider::CubeMapNegativeZ);
    }
}

QUrl Quick3DTextureCubeMapExtension::source() const
{
    return m_source;
}

QUrl Quick3DTextureCubeMapExtension::positiveX() const
{
    return m_positiveX;
}

QUrl Quick3DTextureCubeMapExtension::positiveY() const
{
    return m_positiveY;
}

QUrl Quick3DTextureCubeMapExtension::positiveZ() const
{
    return m_positiveZ;
}

QUrl Quick3DTextureCubeMapExtension::negativeX() const
{
    return m_negativeX;
}

QUrl Quick3DTextureCubeMapExtension::negativeY() const
{
    return m_negativeY;
}

QUrl Quick3DTextureCubeMapExtension::negativeZ() const
{
    return m_negativeZ;
}

// TO DO: To be modified to work with the Texture API refactoring
void Quick3DTextureCubeMapExtension::loadFace(const QUrl &faceUrl, QAbstractTextureProvider::CubeMapFace face)
{
    if (faceUrl.isLocalFile() || faceUrl.scheme() == QStringLiteral("qrc")) {
        QString source = faceUrl.toString().replace(QStringLiteral("qrc"), QStringLiteral(""));
        QImage img;
        // Status will be set by backend

//        parentTexture()->setStatus(QAbstractTextureProvider::Loading);
        if (img.load(source)) {
            TexImageDataPtr dataPtr(new TexImageData());

//            dataPtr->setCubeFace(static_cast<QOpenGLTexture::CubeMapFace>(face));
            if (parentTexture()->height() != img.height())
                parentTexture()->setHeight(img.height());
            if (parentTexture()->width() != img.width())
                parentTexture()->setWidth(img.width());
            QAbstractTextureProvider::TextureFormat format = img.hasAlphaChannel() ?
                        QAbstractTextureProvider::RGBA8_UNorm :
                        QAbstractTextureProvider::RGB8_UNorm;
            if (format != parentTexture()->format())
                parentTexture()->setFormat(format);
            dataPtr->setImage(img);
//            parentTexture()->addImageData(dataPtr);
//            parentTexture()->setStatus(QAbstractTextureProvider::Loaded);
        }
        else {
            qWarning() << "Failed to load image : " << source;
//            parentTexture()->setStatus(QAbstractTextureProvider::Error);
        }
    } else {
//        parentTexture()->setStatus(QAbstractTextureProvider::Error);
        qWarning() << "implement loading from remote URLs";
    }
}

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
