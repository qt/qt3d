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

Quick3DTexture1D::Quick3DTexture1D(QNode *parent)
    : QTexture(Target1D, parent)
{
}

Quick3DTexture1DArray::Quick3DTexture1DArray(QNode *parent)
    : QTexture(Target1DArray, parent)
{
}

Quick3DTexture2D::Quick3DTexture2D(QNode *parent)
    : QTexture(Target2D, parent)
{
}

Quick3DTexture2DArray::Quick3DTexture2DArray(QNode *parent)
    : QTexture(Target2DArray, parent)
{
}

Quick3DTexture3D::Quick3DTexture3D(QNode *parent)
    : QTexture(Target3D, parent)
{
}

Quick3DTextureCubeMap::Quick3DTextureCubeMap(QNode *parent)
    : QTexture(TargetCubeMap, parent)
{
}

Quick3DTextureCubeMapArray::Quick3DTextureCubeMapArray(QNode *parent)
    : QTexture(TargetCubeMapArray, parent)
{
}

Quick3DTexture2DMultisample::Quick3DTexture2DMultisample(QNode *parent)
    : QTexture(Target2DMultisample, parent)
{
}

Quick3DTexture2DMultisampleArray::Quick3DTexture2DMultisampleArray(QNode *parent)
    : QTexture(Target2DMultisampleArray, parent)
{
}

Quick3DTextureRectangle::Quick3DTextureRectangle(QNode *parent)
    : QTexture(TargetRectangle, parent)
{
}

Quick3DTextureBuffer::Quick3DTextureBuffer(QNode *parent)
    : QTexture(TargetBuffer, parent)
{
}

// TO DO: This needs to be reworked to handle more than just 2D images
Quick3DTexture2DExtension::Quick3DTexture2DExtension(QObject *parent)
    : QObject(parent)
{
}

QUrl Quick3DTexture2DExtension::source() const
{
    return m_source;
}

void Quick3DTexture2DExtension::setSource(QUrl arg)
{
    if (m_source != arg) {
        m_source = arg;
        // There is maybe a better way to check for resources files
        if (m_source.isLocalFile() || m_source.scheme() == QStringLiteral("qrc")) {
            QString source = m_source.toString().replace(QStringLiteral("qrc"), QStringLiteral(""));
            QImage img;
            if (img.load(source)) {
                parentTexture()->setFormat(img.hasAlphaChannel() ?
                                               QTexture::RGBA8_UNorm :
                                               QTexture::RGB8_UNorm);
                parentTexture()->setFromQImage(img);
            }
            else {
                qWarning() << "Failed to load image : " << source;
            }

        } else {
            qWarning() << "implement loading from remote URLs";
        }
        emit sourceChanged();
    }
}

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
        loadFace(positiveX, QTexture::CubeMapPositiveX);
    }
}

void Quick3DTextureCubeMapExtension::setPositiveY(const QUrl &positiveY)
{
    if (positiveY != m_positiveY) {
        m_positiveY = positiveY;
        emit positiveYChanged();
        loadFace(positiveY, QTexture::CubeMapPositiveY);
    }
}

void Quick3DTextureCubeMapExtension::setPositiveZ(const QUrl &positiveZ)
{
    if (positiveZ != m_positiveZ) {
        m_positiveZ = positiveZ;
        emit positiveZChanged();
        loadFace(positiveZ, QTexture::CubeMapPositiveZ);
    }
}

void Quick3DTextureCubeMapExtension::setNegativeX(const QUrl &negativeX)
{
    if (negativeX != m_negativeX) {
        m_negativeX = negativeX;
        emit negativeXChanged();
        loadFace(negativeX, QTexture::CubeMapNegativeX);
    }
}

void Quick3DTextureCubeMapExtension::setNegativeY(const QUrl &negativeY)
{
    if (negativeY != m_negativeY) {
        m_negativeY = negativeY;
        emit negativeYChanged();
        loadFace(negativeY, QTexture::CubeMapNegativeY);
    }
}

void Quick3DTextureCubeMapExtension::setNegativeZ(const QUrl &negativeZ)
{
    if (negativeZ != m_negativeZ) {
        m_negativeZ = negativeZ;
        emit negativeZChanged();
        loadFace(negativeZ, QTexture::CubeMapNegativeZ);
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

void Quick3DTextureCubeMapExtension::loadFace(const QUrl &faceUrl, QTexture::CubeMapFace face)
{
    if (faceUrl.isLocalFile() || faceUrl.scheme() == QStringLiteral("qrc")) {
        QString source = faceUrl.toString().replace(QStringLiteral("qrc"), QStringLiteral(""));
        QImage img;
        parentTexture()->setStatus(QTexture::Loading);
        if (img.load(source)) {
            TexImageDataPtr dataPtr(new TexImageData(0, 0));

            dataPtr->setCubeFace(static_cast<QOpenGLTexture::CubeMapFace>(face));
            if (parentTexture()->height() != img.height())
                parentTexture()->setHeight(img.height());
            if (parentTexture()->width() != img.width())
                parentTexture()->setWidth(img.width());
            QTexture::TextureFormat format = img.hasAlphaChannel() ?
                        QTexture::RGBA8_UNorm :
                        QTexture::RGB8_UNorm;
            if (format != parentTexture()->format())
                parentTexture()->setFormat(format);
            dataPtr->setImage(img);
            parentTexture()->addImageData(dataPtr);
            parentTexture()->setStatus(QTexture::Loaded);
        }
        else {
            qWarning() << "Failed to load image : " << source;
            parentTexture()->setStatus(QTexture::Error);
        }
    } else {
        parentTexture()->setStatus(QTexture::Error);
        qWarning() << "implement loading from remote URLs";
    }
}

} // Quick

} // Render

} // Qt3D

QT_END_NAMESPACE
