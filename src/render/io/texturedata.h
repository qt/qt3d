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

#ifndef QT3D_TEXTUREDATA_H
#define QT3D_TEXTUREDATA_H

#include <QOpenGLTexture>
#include <QImage>
#include <QSharedPointer>

QT_BEGIN_NAMESPACE

namespace Qt3D {

class TexImageData
{
public:
    TexImageData(int level, int layer);

    QOpenGLTexture::CubeMapFace cubeFace() const
    { return m_cubeFace; }

    void setCubeFace(QOpenGLTexture::CubeMapFace face);

    int layer() const
    { return m_layer; }

    int mipMapLevel() const
    { return m_mipMapLevel; }

    bool isCompressed() const
    { return m_isCompressed; }

    void setImage(const QImage &);

    void setData(const QByteArray &data,
                 QOpenGLTexture::PixelFormat fmt,
                 QOpenGLTexture::PixelType ptype);
    void setCompressedData(QByteArray data, QOpenGLTexture::PixelFormat fmt);

    QByteArray data() const
    { return m_data; }

    QOpenGLTexture::PixelFormat pixelFormat() const
    { return m_pixelFormat; }

    QOpenGLTexture::PixelType pixelType() const
    { return m_pixelType; }

private:
    int m_layer, m_mipMapLevel;
    QOpenGLTexture::CubeMapFace m_cubeFace;
    QOpenGLTexture::PixelFormat m_pixelFormat;
    QOpenGLTexture::PixelType m_pixelType;

    bool m_isCompressed;
    QByteArray m_data;
};

typedef QSharedPointer<TexImageData> TexImageDataPtr;

} // namespace Qt3D


QT_END_NAMESPACE

#endif // QT3D_TEXTUREDATA_H
