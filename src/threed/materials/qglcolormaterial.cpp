/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtQuick3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qglcolormaterial.h"
#include "qglpainter.h"

QT_BEGIN_NAMESPACE

/*!
    \class QGLColorMaterial
    \since 4.8
    \brief The QGLColorMaterial class implements flat or per-vertex color materials for 3D rendering.
    \ingroup qt3d
    \ingroup qt3d::materials

    When bound to a QGLPainter, QGLColorMaterial will select a flat
    color drawing effect, to render fragments with color(), ignoring
    any lights or textures that may be active on the QGLPainter state.
    If the geometry has the QGL::Color attribute, then a per-vertex
    color will be used instead and color() is ignored.

    \sa QGLMaterial
*/

class QGLColorMaterialPrivate
{
public:
    QGLColorMaterialPrivate() : color(255, 255, 255, 255) {}

    QColor color;
};

/*!
    Constructs a new flat color material and attaches it to \a parent.
*/
QGLColorMaterial::QGLColorMaterial(QObject *parent)
    : QGLAbstractMaterial(parent)
    , d_ptr(new QGLColorMaterialPrivate)
{
}

/*!
    Destroys this flat color material.
*/
QGLColorMaterial::~QGLColorMaterial()
{
}

/*!
    \property QGLColorMaterial::color
    \brief the flat color to use to render the material.  The default
    color is white.

    If the geometry has per-vertex colors, then this property is ignored.

    \sa colorChanged()
*/

QColor QGLColorMaterial::color() const
{
    Q_D(const QGLColorMaterial);
    return d->color;
}

void QGLColorMaterial::setColor(const QColor &color)
{
    Q_D(QGLColorMaterial);
    if (d->color != color) {
        d->color = color;
        emit colorChanged();
        emit materialChanged();
    }
}

/*!
    \reimp
*/
bool QGLColorMaterial::isTransparent() const
{
    Q_D(const QGLColorMaterial);
    return d->color.alpha() != 255;
}

/*!
    \reimp
*/
void QGLColorMaterial::bind(QGLPainter *painter)
{
    Q_D(const QGLColorMaterial);
    painter->setColor(d->color);
    // Effect is set during prepareToDraw().
}

/*!
    \reimp
*/
void QGLColorMaterial::release(QGLPainter *painter, QGLAbstractMaterial *next)
{
    // No textures or other modes, so nothing to do here.
    Q_UNUSED(painter);
    Q_UNUSED(next);
}

/*!
    \reimp
*/
void QGLColorMaterial::prepareToDraw
    (QGLPainter *painter, const QGLAttributeSet &attributes)
{
    if (attributes.contains(QGL::Color))
        painter->setStandardEffect(QGL::FlatPerVertexColor);
    else
        painter->setStandardEffect(QGL::FlatColor);
}

/*!
    \fn void QGLColorMaterial::colorChanged()

    Signal that is emitted when color() changes.

    \sa color()
*/

QT_END_NAMESPACE
