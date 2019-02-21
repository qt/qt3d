/****************************************************************************
**
** Copyright (C) 2019 Klaralvdalens Datakonsult AB (KDAB).
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

#include "qtexturedataupdate.h"
#include "qtexturedataupdate_p.h"
#include <iostream>

QT_BEGIN_NAMESPACE

namespace Qt3DRender
{

QTextureDataUpdatePrivate::QTextureDataUpdatePrivate()
    : m_x(0)
    , m_y(0)
    , m_z(0)
    , m_layer(0)
    , m_mipLevel(0)
    , m_face(QAbstractTexture::CubeMapPositiveX)
{
}

QTextureDataUpdatePrivate::QTextureDataUpdatePrivate(const QTextureDataUpdatePrivate &other)
    : m_x(other.m_x)
    , m_y(other.m_y)
    , m_z(other.m_z)
    , m_layer(other.m_layer)
    , m_mipLevel(other.m_mipLevel)
    , m_face(other.m_face)
    , m_data(other.m_data)
{
}

bool QTextureDataUpdatePrivate::operator==(const QTextureDataUpdatePrivate &other) const
{
    return (m_x == other.m_x && m_y == other.m_y && m_z == other.m_z &&
            m_layer == other.m_layer && m_mipLevel == other.m_mipLevel &&
            m_face == other.m_face && m_data == other.m_data);
}

bool QTextureDataUpdatePrivate::operator!=(const QTextureDataUpdatePrivate &other) const
{
    return !(*this == other);
}

/*!
    \class Qt3DRender::QTextureDataUpdate
    \brief QTextureDataUpdate holds content and information required to perform
    partial updates of a texture content.

    The actual data content is contained in a QTextureImageDataPtr member.
    Additional members allow to specify the x, y, z offset of the content update
    as well as the eventual layer, mipLevel and face.

    \sa QAbstractTexture
 */

QTextureDataUpdate::QTextureDataUpdate()
    : d(new QTextureDataUpdatePrivate())
{
}

QTextureDataUpdate::QTextureDataUpdate(const QTextureDataUpdate &other)
    : d(new QTextureDataUpdatePrivate(*other.d))
{
}

QTextureDataUpdate &QTextureDataUpdate::operator=(const QTextureDataUpdate &other)
{
    QTextureDataUpdate tmp(other);
    d.swap(tmp.d);
    return *this;
}

QTextureDataUpdate::~QTextureDataUpdate()
{
}

bool QTextureDataUpdate::operator==(const QTextureDataUpdate &other) const
{
    return *d == *other.d;
}

bool QTextureDataUpdate::operator!=(const QTextureDataUpdate &other) const
{
    return *d != *other.d;
}

int QTextureDataUpdate::x() const
{
    return d->m_x;
}

int QTextureDataUpdate::y() const
{
    return d->m_y;
}

int QTextureDataUpdate::z() const
{
    return d->m_z;
}

int QTextureDataUpdate::layer() const
{
    return d->m_layer;
}

int QTextureDataUpdate::mipLevel() const
{
    return d->m_mipLevel;
}

QAbstractTexture::CubeMapFace QTextureDataUpdate::face() const
{
    return d->m_face;
}

QTextureImageDataPtr QTextureDataUpdate::data() const
{
    return d->m_data;
}

void QTextureDataUpdate::setX(int x)
{
    d->m_x = x;
}

void QTextureDataUpdate::setY(int y)
{
    d->m_y = y;
}

void QTextureDataUpdate::setZ(int z)
{
    d->m_z = z;
}

void QTextureDataUpdate::setLayer(int layer)
{
    d->m_layer = layer;
}

void QTextureDataUpdate::setMipLevel(int mipLevel)
{
    d->m_mipLevel = mipLevel;
}

void QTextureDataUpdate::setFace(QAbstractTexture::CubeMapFace face)
{
    d->m_face = face;
}

void QTextureDataUpdate::setData(const QTextureImageDataPtr &data)
{
    d->m_data = data;
}

} // Qt3DRender

QT_END_NAMESPACE
