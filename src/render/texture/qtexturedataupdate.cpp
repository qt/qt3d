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
