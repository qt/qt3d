/****************************************************************************
**
** Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
******************************************************************************/

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
