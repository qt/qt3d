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

#include "texture.h"

#include <texturedata.h>

#include <QImage>
#include <QDebug>

QT_BEGIN_NAMESPACE

namespace Qt3D {

class TexturePrivate
{
public :
    TexturePrivate(Texture *qq)
        : q_ptr(qq)
        , m_target(Texture::Target2D)
        , m_format(Texture::RGBA8U)
        , m_width(1)
        , m_height(1)
        , m_depth(1)
        , m_autoMipMap(false)
        , m_minFilter(Texture::Nearest)
        , m_magFilter(Texture::Nearest)
        , m_wrapMode(Texture::ClampToEdge)
        , m_status(Texture::Loading)
        , m_uuid(QUuid::createUuid())
    {}

    Q_DECLARE_PUBLIC(Texture)
    Texture *q_ptr;

    Texture::Target m_target;
    Texture::TextureFormat m_format;
    int m_width, m_height, m_depth;
    bool m_autoMipMap;

    QList<TexImageDataPtr> m_data;

    Texture::Filter m_minFilter, m_magFilter;
    // FIXME, store per direction
    Texture::WrapMode m_wrapMode;
    Texture::Status m_status;
    const QUuid m_uuid;
};


Texture::Texture(QObject *parent)
    : QObject(parent)
    , d_ptr(new TexturePrivate(this))
{
}

Texture::~Texture()
{
    delete d_ptr;
}

const QUuid Texture::uuid() const
{
    Q_D(const Texture);
    return d->m_uuid;
}

void Texture::setTarget(Target target)
{
    Q_D(Texture);
    if (d->m_target != target) {
        d->m_target = target;
    }
}

void Texture::setSize(int w, int h, int d)
{
   setWidth(w);
   setHeight(h);
   setDepth(d);
}

void Texture::setWidth(int width)
{
    Q_D(Texture);
    if (d->m_width != width) {
        d->m_width = width;
        emit widthChanged();
    }
}

void Texture::setHeight(int height)
{
    Q_D(Texture);
    if (d->m_height != height) {
        d->m_height = height;
        emit heightChanged();
    }
}

void Texture::setDepth(int depth)
{
    Q_D(Texture);
    if (d->m_depth != depth) {
        d->m_depth = depth;
        emit depthChanged();
    }
}

int Texture::width() const
{
    Q_D(const Texture);
    return d->m_width;
}

int Texture::height() const
{
    Q_D(const Texture);
    return d->m_height;
}

int Texture::depth() const
{
    Q_D(const Texture);
    return d->m_depth;
}

void Texture::setInternalFormat(TextureFormat format)
{
    Q_D(Texture);
    if (d->m_format != format) {
        d->m_format = format;
        emit formatChanged();
    }
}

Texture::TextureFormat Texture::format() const
{
    Q_D(const Texture);
    return d->m_format;
}

void Texture::setStatus(Status status)
{
    Q_D(Texture);
    if (status != d->m_status) {
        d->m_status = status;
        emit statusChanged();
    }
}

Texture::Status Texture::status() const
{
    Q_D(const Texture);
    return d->m_status;
}

Texture::Target Texture::target() const
{
    Q_D(const Texture);
    return d->m_target;
}

bool Texture::setFromQImage(QImage img, int layer)
{
    Q_D(Texture);
    setSize(img.width(), img.height());

    if ((d->m_target != Target2D) &&
        (d->m_target != Target2DArray) &&
        (d->m_target == TargetRectangle))
    {
        qWarning() << Q_FUNC_INFO << "invalid texture target";
        setStatus(Error);
        return false;
    }

    TexImageDataPtr dataPtr(new TexImageData(0, layer));
    dataPtr->setImage(img);
    addImageData(dataPtr);
    setStatus(Loaded);
    return true;
}

void Texture::addImageData(TexImageDataPtr imgData)
{
    Q_D(Texture);
    d->m_data.append(imgData);
}

QList<TexImageDataPtr> Texture::imageData() const
{
    Q_D(const Texture);
    return d->m_data;
}

void Texture::setGenerateMipMaps(bool gen)
{
    Q_D(Texture);
    if (d->m_autoMipMap != gen) {
        d->m_autoMipMap = gen;
        emit generateMipMapsChanged();
    }
}

bool Texture::generateMipMaps() const
{
    Q_D(const Texture);
    return d->m_autoMipMap;
}

void Texture::setMinificationFilter(Filter f)
{
    Q_D(Texture);
    if (d->m_minFilter != f) {
        d->m_minFilter = f;
        emit minificationFilterChanged();
    }
}

void Texture::setMagnificationFilter(Filter f)
{
    Q_D(Texture);
    if (d->m_magFilter != f) {
        d->m_magFilter = f;
        emit magnificationFilterChanged();
    }
}

Texture::Filter Texture::minificationFilter() const
{
    Q_D(const Texture);
    return d->m_minFilter;
}

Texture::Filter Texture::magnificationFilter() const
{
    Q_D(const Texture);
    return d->m_magFilter;
}

void Texture::setWrapMode(WrapMode wrapMode)
{
    Q_D(Texture);
    if (d->m_wrapMode != wrapMode) {
        d->m_wrapMode = wrapMode;
        emit wrapModeChanged();
    }
}

Texture::WrapMode Texture::wrapMode() const
{
    Q_D(const Texture);
    return d->m_wrapMode;
}

} // namespace Qt3D

QT_END_NAMESPACE
