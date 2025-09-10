// Copyright (C) 2019 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qtexturedataupdate.h"
#include "qtexturedataupdate_p.h"
#include <iostream>

QT_BEGIN_NAMESPACE

namespace Qt3DRender
{

static bool operator==(const QTextureDataUpdatePrivate &lhs, const QTextureDataUpdatePrivate &rhs) noexcept
{
    return lhs.m_x == rhs.m_x &&
           lhs.m_y == rhs.m_y &&
           lhs.m_z == rhs.m_z &&
           lhs.m_layer == rhs.m_layer &&
           lhs.m_mipLevel == rhs.m_mipLevel &&
           lhs.m_face == rhs.m_face &&
           lhs.m_data == rhs.m_data;
}

/*!
    \class Qt3DRender::QTextureDataUpdate
    \inmodule Qt3DRender
    \brief QTextureDataUpdate holds content and information required to perform
    partial updates of a texture content.

    The actual data content is contained in a QTextureImageDataPtr member.
    Additional members allow to specify the x, y, z offset of the content update
    as well as the eventual layer, mipLevel and face.

    \sa QAbstractTexture
    \since 5.14
 */

QTextureDataUpdate::QTextureDataUpdate()
    : d_ptr(new QTextureDataUpdatePrivate())
{
}

QTextureDataUpdate::QTextureDataUpdate(const QTextureDataUpdate &other)
    = default;

QTextureDataUpdate &QTextureDataUpdate::operator=(const QTextureDataUpdate &other)
    = default;

QTextureDataUpdate::~QTextureDataUpdate()
    = default;

bool operator==(const QTextureDataUpdate &lhs, const QTextureDataUpdate &rhs) noexcept
{
    return *lhs.d_func() == *rhs.d_func();
}

int QTextureDataUpdate::x() const
{
    Q_D(const QTextureDataUpdate);
    return d->m_x;
}

int QTextureDataUpdate::y() const
{
    Q_D(const QTextureDataUpdate);
    return d->m_y;
}

int QTextureDataUpdate::z() const
{
    Q_D(const QTextureDataUpdate);
    return d->m_z;
}

int QTextureDataUpdate::layer() const
{
    Q_D(const QTextureDataUpdate);
    return d->m_layer;
}

int QTextureDataUpdate::mipLevel() const
{
    Q_D(const QTextureDataUpdate);
    return d->m_mipLevel;
}

QAbstractTexture::CubeMapFace QTextureDataUpdate::face() const
{
    Q_D(const QTextureDataUpdate);
    return d->m_face;
}

QTextureImageDataPtr QTextureDataUpdate::data() const
{
    Q_D(const QTextureDataUpdate);
    return d->m_data;
}

void QTextureDataUpdate::setX(int x)
{
    d_ptr.detach();
    Q_D(QTextureDataUpdate);
    d->m_x = x;
}

void QTextureDataUpdate::setY(int y)
{
    d_ptr.detach();
    Q_D(QTextureDataUpdate);
    d->m_y = y;
}

void QTextureDataUpdate::setZ(int z)
{
    d_ptr.detach();
    Q_D(QTextureDataUpdate);
    d->m_z = z;
}

void QTextureDataUpdate::setLayer(int layer)
{
    d_ptr.detach();
    Q_D(QTextureDataUpdate);
    d->m_layer = layer;
}

void QTextureDataUpdate::setMipLevel(int mipLevel)
{
    d_ptr.detach();
    Q_D(QTextureDataUpdate);
    d->m_mipLevel = mipLevel;
}

void QTextureDataUpdate::setFace(QAbstractTexture::CubeMapFace face)
{
    d_ptr.detach();
    Q_D(QTextureDataUpdate);
    d->m_face = face;
}

void QTextureDataUpdate::setData(const QTextureImageDataPtr &data)
{
    d_ptr.detach();
    Q_D(QTextureDataUpdate);
    d->m_data = data;
}

} // Qt3DRender

QT_END_NAMESPACE
