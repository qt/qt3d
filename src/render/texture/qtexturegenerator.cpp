// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qtexturegenerator_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

/*! class Qt3DRender::QTextureGenerator
    \inmodule Qt3DRender
    \brief Generates the necessary information to create an OpenGL texture image.
*/
QTextureGenerator::~QTextureGenerator()
{
}

/*!
   \class Qt3DRender::QTextureGenerator
   \inmodule Qt3DRender
   \brief Provides the image data for a texture.
*/
/*!
    \fn QTextureDataPtr Qt3DRender::QTextureGenerator::operator()()

    Returns the texture data.
*/
/*!
    \fn bool Qt3DRender::QTextureGenerator::operator==(const QTextureGenerator &other) const = 0

    Compare these texture data to \a other. Returns \c true if both are equal.
*/
} // Qt3DRender

QT_END_NAMESPACE
