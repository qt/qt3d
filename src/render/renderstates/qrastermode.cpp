// Copyright (C) 2018 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qrastermode.h"
#include "qrastermode_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

/*!
    \class Qt3DRender::QRasterMode
    \brief The QRasterMode render state allows to control the type of
           rasterization to be performed.
    \since 5.14
    \inmodule Qt3DRender
    \ingroup renderstates

    The QRasterMode class is used to control the rasterization step of the
    primitives at render time. This can be used to choose whether we only
    want to show points, edges or fill a primitive.

    \note This is not supported when rendering on OpenGL ES 2.0 platforms.

    \sa QAlphaTest, QStencilTest
 */

/*!
    \qmltype RasterMode
    \brief The RasterMode render state allows to control the type of
           rasterization to be performed.
    \since 5.14
    \inqmlmodule Qt3D.Render
    \inherits RenderState
    \instantiates Qt3DRender::QRasterMode
    \ingroup renderstates

    The QRasterMode class is used to control the rasterization step of the
    primitives at render time. This can be used to choose whether we only
    want to show points, edges or fill a primitive.

    \note This is not supported when rendering on OpenGL ES 2.0 platforms.

    \sa AlphaTest, StencilTest
 */

/*!
    \enum Qt3DRender::QRasterMode::RasterMode

    Enumeration for raster mode values
    \value Points Vertices at the start of an edge are drawn as points.
    \value Lines Edges of a polygon are draw as line segments.
    \value Fill Fills the interior of the primitive.
*/

/*!
    \enum Qt3DRender::QRasterMode::FaceMode

    Enumeration for face mode values
    \value Front Applies to front faces only
    \value Back Applies to back faces only
    \value FrontAndBack Applies to front and back faces
*/

/*!
    \property QRasterMode::rasterMode

    Holds the raster mode to be used.
*/

/*!
    \property QRasterMode::faceMode

    Holds the face mode to be used. Controls on which face the raster mode is
    to be applied.
*/

/*!
    \qmlproperty enumeration RasterMode::rasterMode

    Holds the raster mode to be used.

    \list
    \li Points Vertices at the start of an edge are drawn as points.
    \li Lines Edges of a polygon are draw as line segments.
    \li Fill Fills the interior of the primitive.
    \endlist
*/

/*!
    \qmlproperty enumeration RasterMode::faceMode

    Holds the face mode to be used. Controls on which face the raster mode is
    to be applied.

    \list
    \li Front Applies to front faces only
    \li Back Applies to back faces only
    \li FrontAndBack Applies to front and back faces
    \endlist
*/



QRasterMode::QRasterMode(QNode *parent)
    : QRenderState(*new QRasterModePrivate, parent)
{
}

/*!
    \internal
*/
QRasterMode::~QRasterMode()
    = default;

QRasterMode::RasterMode QRasterMode::rasterMode() const
{
    Q_D(const QRasterMode);
    return d->m_rasterMode;
}

QRasterMode::FaceMode QRasterMode::faceMode() const
{
    Q_D(const QRasterMode);
    return d->m_faceMode;
}

void QRasterMode::setRasterMode(QRasterMode::RasterMode rasterMode)
{
    Q_D(QRasterMode);
    if (d->m_rasterMode != rasterMode) {
        d->m_rasterMode = rasterMode;
        emit rasterModeChanged(rasterMode);
    }
}

void QRasterMode::setFaceMode(QRasterMode::FaceMode faceMode)
{
    Q_D(QRasterMode);
    if (d->m_faceMode != faceMode) {
        d->m_faceMode = faceMode;
        emit faceModeChanged(faceMode);
    }
}

} // namespace Qt3DRender

QT_END_NAMESPACE

#include "moc_qrastermode.cpp"
