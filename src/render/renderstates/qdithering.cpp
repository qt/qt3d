// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// Copyright (C) 2016 The Qt Company Ltd and/or its subsidiary(-ies).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qdithering.h"
#include "qrenderstate_p.h"
#include <private/qnode_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

/*!
    \class Qt3DRender::QDithering
    \since 5.7
    \inmodule Qt3DRender
    \brief Enable dithering.

    A Qt3DRender::QDithering class enables dithering. Dithering adds noise to the
    color values to randomize quantization error in order to prevent large scale
    patterns in the final image, such as banding. Dithering is most useful when
    rendering to a surface with low color bit depth, such as RGB565 or RGBA4444.
 */

/*!
    \qmltype Dithering
    \since 5.7
    \inqmlmodule Qt3D.Render
    \inherits RenderState
    \nativetype Qt3DRender::QDithering
    \brief Enable dithering.

    A Dithering type enables dithering. Dithering adds noise to the
    color values to randomize quantization error in order to prevent large scale
    patterns in the final image, such as banding. Dithering is most useful when
    rendering to a surface with low color bit depth, such as RGB565 or RGBA4444.
 */

class QDitheringPrivate : public QRenderStatePrivate
{
public:
    QDitheringPrivate()
        : QRenderStatePrivate(Render::DitheringStateMask)
    {}

    Q_DECLARE_PUBLIC(QDithering)
};

/*!
    The constructor creates a new QDithering::QDithering instance with
    the specified \a parent.
 */
QDithering::QDithering(QNode *parent)
    : QRenderState(*new QDitheringPrivate, parent)
{
}

/*! \internal */
QDithering::~QDithering()
{
}

} // namespace Qt3DRender

QT_END_NAMESPACE

#include "moc_qdithering.cpp"
