/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
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

#include "qtextureproviders.h"

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

/*!
    \class Qt3DRender::QTexture1D
    \inmodule Qt3DRender
    \since 5.5
    \brief A QAbstractTextureProvider with a Target1D target format.
 */

/*!
    Constructs a new Qt3DRender::QTexture1D instance with \a parent as parent.
 */
QTexture1D::QTexture1D(QNode *parent)
    : QAbstractTextureProvider(Target1D, parent)
{
}

QTexture1D::~QTexture1D()
{
}

/*!
    \class Qt3DRender::QTexture1DArray
    \inmodule Qt3DRender
    \since 5.5
    \brief A QAbstractTextureProvider with a Target1DArray target format.
 */

/*!
    Constructs a new Qt3DRender::QTexture1DArray instance with \a parent as parent.
 */
QTexture1DArray::QTexture1DArray(QNode *parent)
    : QAbstractTextureProvider(Target1DArray, parent)
{
}

QTexture1DArray::~QTexture1DArray()
{
}

/*!
    \class Qt3DRender::QTexture2D
    \inmodule Qt3DRender
    \since 5.5
    \brief A QAbstractTextureProvider with a Target2D target format.
 */

/*!
    Constructs a new Qt3DRender::QTexture2D instance with \a parent as parent.
 */
QTexture2D::QTexture2D(QNode *parent)
    : QAbstractTextureProvider(Target2D, parent)
{
}

QTexture2D::~QTexture2D()
{
}

/*!
    \class Qt3DRender::QTexture2DArray
    \inmodule Qt3DRender
    \since 5.5
    \brief A QAbstractTextureProvider with a Target2DArray target format.
 */

/*!
    Constructs a new Qt3DRender::QTexture2DArray instance with \a parent as parent.
 */
QTexture2DArray::QTexture2DArray(QNode *parent)
    : QAbstractTextureProvider(Target2DArray, parent)
{
}

QTexture2DArray::~QTexture2DArray()
{
}


/*!
    \class Qt3DRender::QTexture3D
    \inmodule Qt3DRender
    \since 5.5
    \brief A QAbstractTextureProvider with a Target3D target format.
 */

/*!
    Constructs a new Qt3DRender::QTexture3D instance with \a parent as parent.
 */
QTexture3D::QTexture3D(QNode *parent)
    : QAbstractTextureProvider(Target3D, parent)
{
}

QTexture3D::~QTexture3D()
{
}

/*!
    \class Qt3DRender::QTextureCubeMap
    \inmodule Qt3DRender
    \since 5.5
    \brief A QAbstractTextureProvider with a TargetCubeMap target format.
 */

/*!
    Constructs a new Qt3DRender::QTextureCubeMap instance with \a parent as parent.
 */
QTextureCubeMap::QTextureCubeMap(QNode *parent)
    : QAbstractTextureProvider(TargetCubeMap, parent)
{
}

QTextureCubeMap::~QTextureCubeMap()
{
}

/*!
    \class Qt3DRender::QTextureCubeMapArray
    \inmodule Qt3DRender
    \since 5.5
    \brief A QAbstractTextureProvider with a TargetCubeMapArray target format.
 */

/*!
    Constructs a new Qt3DRender::QTextureCubeMapArray instance with \a parent as parent.
 */
QTextureCubeMapArray::QTextureCubeMapArray(QNode *parent)
    : QAbstractTextureProvider(TargetCubeMapArray, parent)
{
}

QTextureCubeMapArray::~QTextureCubeMapArray()
{
    QNode::cleanup();
}

/*!
    \class Qt3DRender::QTexture2DMultisample
    \inmodule Qt3DRender
    \since 5.5
    \brief A QAbstractTextureProvider with a Target2DMultisample target format.
 */

/*!
    Constructs a new Qt3DRender::QTexture2DMultisample instance with \a parent as parent.
 */
QTexture2DMultisample::QTexture2DMultisample(QNode *parent)
    : QAbstractTextureProvider(Target2DMultisample, parent)
{
}

QTexture2DMultisample::~QTexture2DMultisample()
{
}

/*!
    \class Qt3DRender::QTexture2DMultisampleArray
    \inmodule Qt3DRender
    \since 5.5
    \brief A QAbstractTextureProvider with a Target2DMultisampleArray target format.
 */

/*!
    Constructs a new Qt3DRender::QTexture2DMultisampleArray instance with \a parent as parent.
 */
QTexture2DMultisampleArray::QTexture2DMultisampleArray(QNode *parent)
    : QAbstractTextureProvider(Target2DMultisampleArray, parent)
{
}

QTexture2DMultisampleArray::~QTexture2DMultisampleArray()
{
}

/*!
    \class Qt3DRender::QTextureRectangle
    \inmodule Qt3DRender
    \since 5.5
    \brief A QAbstractTextureProvider with a TargetRectangle target format.
 */

/*!
    Constructs a new Qt3DRender::QTextureRectangle instance with \a parent as parent.
 */
QTextureRectangle::QTextureRectangle(QNode *parent)
    : QAbstractTextureProvider(TargetRectangle, parent)
{
}

QTextureRectangle::~QTextureRectangle()
{
}

/*!
    \class Qt3DRender::QTextureBuffer
    \inmodule Qt3DRender
    \since 5.5
    \brief A QAbstractTextureProvider with a TargetBuffer target format.
 */

/*!
    Constructs a new Qt3DRender::QTextureBuffer instance with \a parent as parent.
 */
QTextureBuffer::QTextureBuffer(QNode *parent)
    : QAbstractTextureProvider(TargetBuffer, parent)
{
}

QTextureBuffer::~QTextureBuffer()
{
}

} // namespace Qt3DRender

QT_END_NAMESPACE


