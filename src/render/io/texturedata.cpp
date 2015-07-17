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

#include "texturedata.h"

#include <QDebug>
#include <QOpenGLTexture>

QT_BEGIN_NAMESPACE

namespace Qt3D {

TexImageData::TexImageData()
    : m_width(-1)
    , m_height(-1)
    , m_depth(-1)
    , m_isCompressed(false)
    , m_format(QOpenGLTexture::RGBA8_UNorm)
{
}

TexImageData::~TexImageData()
{
}

void TexImageData::cleanup()
{
    m_width = -1;
    m_height = -1;
    m_depth = -1;
    m_isCompressed = false;
    m_data.clear();
}

void TexImageData::setImage(const QImage &image)
{
    m_width = image.width();
    m_height = image.height();
    m_depth = 1;

    QImage glImage = image.convertToFormat(QImage::Format_RGBA8888);
    QByteArray imageBytes((const char*) glImage.constBits(), glImage.byteCount());
    setData(imageBytes, QOpenGLTexture::RGBA, QOpenGLTexture::UInt8);
    m_format = QOpenGLTexture::RGBA8_UNorm;
}

void TexImageData::setData(const QByteArray &data, QOpenGLTexture::PixelFormat fmt, QOpenGLTexture::PixelType ptype)
{
    m_isCompressed = false;
    m_data = data;
    m_pixelFormat = fmt;
    m_pixelType = ptype;
}

} // namespace Qt3D

QT_END_NAMESPACE
