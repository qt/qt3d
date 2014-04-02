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

#include "texturedata.h"

#include <QDebug>
#include <QOpenGLTexture>

QT_BEGIN_NAMESPACE

namespace Qt3D {

Texture::Texture() :
    m_target(QOpenGLTexture::Target2D),
    m_width(1),
    m_height(1),
    m_depth(1),
    m_autoMipMap(false)
{
}

void Texture::setTarget(QOpenGLTexture::Target target)
{
    m_target = target;
}

void Texture::setSize(int w, int h, int d)
{
    m_width = w;
    m_height = h;
    m_depth = d;
}

int Texture::width() const
{
    return m_width;
}

int Texture::height() const
{
    return m_height;
}

int Texture::depth() const
{
    return m_depth;
}

void Texture::setInternalFormat(QOpenGLTexture::TextureFormat format)
{
    m_format = format;
}

bool Texture::setFromQImage(QImage img, int layer)
{
    setSize(img.width(), img.height());

    if ((m_target != QOpenGLTexture::Target2D) &&
        (m_target != QOpenGLTexture::Target2DArray) &&
        (m_target == QOpenGLTexture::TargetRectangle))
    {
        qWarning() << Q_FUNC_INFO << "invalid texture target";
        return false;
    }

    TexImageDataPtr dataPtr(new TexImageData(0, layer));
    dataPtr->setImage(img);
    addImageData(dataPtr);

    return true;
}

void Texture::addImageData(TexImageDataPtr imgData)
{
    m_data.append(imgData);
}

QList<TexImageDataPtr> Texture::imageData() const
{
    return m_data;
}

void Texture::setGenerateMipMaps(bool gen)
{
    m_autoMipMap = gen;
}

void Texture::setMinificationFilter(QOpenGLTexture::Filter f)
{
    m_minFilter = f;
}

void Texture::setMagnificationFilter(QOpenGLTexture::Filter f)
{
    m_magFilter = f;
}

void Texture::setWrapMode(QOpenGLTexture::WrapMode wrapMode)
{
    m_wrapMode = wrapMode;
}

/////////////////////////////////////////////////////////////////////////////

TexImageData::TexImageData(int level, int layer) :
    m_layer(layer),
    m_mipMapLevel(level),
    m_cubeFace(QOpenGLTexture::CubeMapPositiveX),
    m_isCompressed(false)
{

}

void TexImageData::setImage(QImage image)
{
    QImage glImage = image.convertToFormat(QImage::Format_RGBA8888);
//    setData(0, QOpenGLTexture::RGBA, QOpenGLTexture::UInt8, glImage.bits(), &uploadOptions);

    QByteArray imageBytes((const char*) glImage.constBits(), glImage.byteCount());
    setData(imageBytes, QOpenGLTexture::RGBA, QOpenGLTexture::UInt8);
}

void TexImageData::setData(QByteArray data, QOpenGLTexture::PixelFormat fmt, QOpenGLTexture::PixelType ptype)
{
    m_isCompressed = false;
    m_data = data;
    m_pixelFormat = fmt;
    m_pixelType = ptype;
}

} // namespace Qt3D

QT_END_NAMESPACE
