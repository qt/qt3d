/****************************************************************************
**
** Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
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


#include "qtexturedata.h"

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QTextureDataPrivate
{
public:
    QAbstractTexture::Target m_target;
    QAbstractTexture::TextureFormat m_format;
    int m_width = 0;
    int m_height = 0;
    int m_depth = 0;
    int m_layers = 0;
    bool m_autoMipMap = false;
    float m_maximumAnisotropy = 0.0f;
    QAbstractTexture::Filter m_minFilter;
    QAbstractTexture::Filter m_magFilter;
    QTextureWrapMode::WrapMode m_wrapModeX;
    QTextureWrapMode::WrapMode m_wrapModeY;
    QTextureWrapMode::WrapMode m_wrapModeZ;
    QAbstractTexture::ComparisonFunction m_comparisonFunction;
    QAbstractTexture::ComparisonMode m_comparisonMode;
    QVector<QTextureImageDataPtr> m_imagesData;

};

QTextureData::QTextureData()
    : d_ptr(new QTextureDataPrivate())
{
}

QTextureData::~QTextureData()
{
    delete d_ptr;
}

QAbstractTexture::Target QTextureData::target() const
{
    Q_D(const QTextureData);
    return d->m_target;
}

void QTextureData::setTarget(QAbstractTexture::Target target)
{
    Q_D(QTextureData);
    d->m_target = target;
}

QAbstractTexture::TextureFormat QTextureData::format() const
{
    Q_D(const QTextureData);
    return d->m_format;
}

void QTextureData::setFormat(QAbstractTexture::TextureFormat format)
{
    Q_D(QTextureData);
    d->m_format = format;
}

int QTextureData::width() const
{
    Q_D(const QTextureData);
    return d->m_width;
}

void QTextureData::setWidth(int width)
{
    Q_D(QTextureData);
    d->m_width = width;
}

int QTextureData::height() const
{
    Q_D(const QTextureData);
    return d->m_height;
}

void QTextureData::setHeight(int height)
{
    Q_D(QTextureData);
    d->m_height = height;
}

int QTextureData::depth() const
{
    Q_D(const QTextureData);
    return d->m_depth;
}

void QTextureData::setDepth(int depth)
{
    Q_D(QTextureData);
    d->m_depth = depth;
}

int QTextureData::layers() const
{
    Q_D(const QTextureData);
    return d->m_layers;
}

void QTextureData::setLayers(int layers)
{
    Q_D(QTextureData);
    d->m_layers = layers;
}

bool QTextureData::isAutoMipMapGenerationEnabled() const
{
    Q_D(const QTextureData);
    return d->m_autoMipMap;
}

void QTextureData::setAutoMipMapGenerationEnabled(bool autoMipMap)
{
    Q_D(QTextureData);
    d->m_autoMipMap = autoMipMap;
}

float QTextureData::maximumAnisotropy() const
{
    Q_D(const QTextureData);
    return d->m_maximumAnisotropy;
}

void QTextureData::setMaximumAnisotropy(float maximumAnisotropy)
{
    Q_D(QTextureData);
    d->m_maximumAnisotropy = maximumAnisotropy;
}

QAbstractTexture::Filter QTextureData::minificationFilter() const
{
    Q_D(const QTextureData);
    return d->m_minFilter;
}

void QTextureData::setMinificationFilter(QAbstractTexture::Filter filter)
{
    Q_D(QTextureData);
    d->m_minFilter = filter;
}

QAbstractTexture::Filter QTextureData::magnificationFilter() const
{
    Q_D(const QTextureData);
    return d->m_magFilter;
}

void QTextureData::setMagnificationFilter(QAbstractTexture::Filter filter)
{
    Q_D(QTextureData);
    d->m_magFilter = filter;
}

QTextureWrapMode::WrapMode QTextureData::wrapModeX() const
{
    Q_D(const QTextureData);
    return d->m_wrapModeX;
}

void QTextureData::setWrapModeX(QTextureWrapMode::WrapMode wrapModeX)
{
    Q_D(QTextureData);
    d->m_wrapModeX = wrapModeX;
}

QTextureWrapMode::WrapMode QTextureData::wrapModeY() const
{
    Q_D(const QTextureData);
    return d->m_wrapModeY;
}

void QTextureData::setWrapModeY(QTextureWrapMode::WrapMode wrapModeY)
{
    Q_D(QTextureData);
    d->m_wrapModeY = wrapModeY;
}

QTextureWrapMode::WrapMode QTextureData::wrapModeZ() const
{
    Q_D(const QTextureData);
    return d->m_wrapModeZ;
}

void QTextureData::setWrapModeZ(QTextureWrapMode::WrapMode wrapModeZ)
{
    Q_D(QTextureData);
    d->m_wrapModeZ = wrapModeZ;
}

QAbstractTexture::ComparisonFunction QTextureData::comparisonFunction() const
{
    Q_D(const QTextureData);
    return d->m_comparisonFunction;
}

void QTextureData::setComparisonFunction(QAbstractTexture::ComparisonFunction comparisonFunction)
{
    Q_D(QTextureData);
    d->m_comparisonFunction = comparisonFunction;
}

QAbstractTexture::ComparisonMode QTextureData::comparisonMode() const
{
    Q_D(const QTextureData);
    return d->m_comparisonMode;
}

void QTextureData::setComparisonMode(QAbstractTexture::ComparisonMode comparisonMode)
{
    Q_D(QTextureData);
    d->m_comparisonMode = comparisonMode;
}

QVector<QTextureImageDataPtr> QTextureData::imageData() const
{
    Q_D(const QTextureData);
    return d->m_imagesData;
}

void QTextureData::addImageData(const QTextureImageDataPtr &imageData)
{
    Q_D(QTextureData);
    d->m_imagesData.push_back(imageData);
}

} // Qt3DRender

QT_END_NAMESPACE
